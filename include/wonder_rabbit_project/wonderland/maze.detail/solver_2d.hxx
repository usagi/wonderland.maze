#pragma once

#include <string>
#include <memory>
#include <random>
#include <type_traits>
#include <sstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <deque>
#include <cmath>

#include <boost/optional.hpp>

#include <glm/glm.hpp>

#include "traits.hxx"

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace maze
    {
      namespace solver
      {
        template
        < class T_size = std::int_fast32_t
        >
        class solver_2d_t
          : public std::enable_shared_from_this< solver_2d_t< T_size > >
        {
        public:
          using size_t = T_size;
          
          using shared_t = std::shared_ptr< solver_2d_t< T_size > >;
          
          using coordinate_t = to_glm_vec2_t< size_t >;
          
          using data_t = std::vector< std::vector< std::uint8_t > >;
          using shared_data_t = std::shared_ptr< data_t >;
          
          using answer_t = std::deque< coordinate_t >;
          using shared_answer_t = std::shared_ptr< answer_t >;
          
        private:
          
          shared_data_t _maze;
          shared_answer_t _answer;
          
          size_t _width;
          size_t _height;
          
          bool _full_gamut;
          
          const std::vector< coordinate_t > _directions;
          
          solver::algorithm _algorithm;
          
          auto solve_find_cell( coordinate_t* pstart = nullptr, coordinate_t* pgoal = nullptr ) const
            -> void
          {
            bool start_finded = false;
            bool goal_finded  = false;
            
            for ( size_t y = 0; y < _maze -> size(); ++y )
              for ( size_t x = 0; x < (*_maze)[y].size(); ++x )
              {
                if ( start( (*_maze)[y][x] ) and pstart )
                {
                  *pstart = { x, y };
                  
                  if ( not pgoal )
                    return;
                  
                  start_finded = true;
                }
                else if ( goal( (*_maze)[y][x] ) and pgoal )
                {
                  *pgoal = { x, y };
                  
                  if ( not pstart )
                    return;
                  
                  goal_finded = true;
                }
              }
            
            if ( pstart and not start_finded )
              throw std::runtime_error( "maze data has not start cell." );
            
            if ( pgoal and not goal_finded )
              throw std::runtime_error( "maze data has not goal cell." );
          }
          
        public:
          
          solver_2d_t()
            : _full_gamut( false )
            , _directions( generate_directions< coordinate_t >() )
            , _algorithm( solver::algorithm::dijkstra )
          { }
          
          auto algorithm( const solver::algorithm a )
            -> shared_t
          {
            _algorithm = a;
            return this ->shared_from_this();
          }
          
          auto full_gamut( bool enable = true )
            -> shared_t
          {
            _full_gamut = enable;
            
            return this -> shared_from_this();
          }
          
          auto load( std::istream& s )
            -> shared_t
          {
            throw std::logic_error( "not impl yet." );
            return this -> shared_from_this();
          }
          
          auto load( const std::string& t )
            -> shared_t
          {
            throw std::logic_error( "not impl yet." );
            return this -> shared_from_this();
          }
          
          auto load( const shared_data_t d )
            -> shared_t
          {
            if ( not d )
              throw std::runtime_error( "maze data is null." );
            
            if ( d -> empty() )
              throw std::runtime_error( "maze data rows is empty." );
            
            if ( d -> cbegin() -> empty() )
              throw std::runtime_error( "maze data cols is empty." );
            
            _maze   = d;
            
            _width  = d -> cbegin() -> size();
            _height = d -> size();
            
            for ( auto row = _maze -> cbegin() + 1; row != _maze -> cend(); ++row )
              if ( _width != row -> size() )
                throw std::runtime_error( "maze data cols is not constant size." );
            
            return this -> shared_from_this();
          }
          
          auto solve( )
            -> shared_t
          {
            struct vertex_data_t
            {
              size_t       distance;
              coordinate_t previous;
            };
            
            std::vector< std::vector< boost::optional< vertex_data_t > > > data
              ( _maze -> size()
              , std::vector< boost::optional< vertex_data_t > >
                ( (*_maze)[0].size()
                )
              );
            
            std::function< auto ( const coordinate_t& p ) -> void > search;
            
            coordinate_t start, goal;
            
            std::function< auto ( const coordinate_t& p, const coordinate_t& np ) -> size_t > distance;
            
            switch( _algorithm )
            { case solver::algorithm::dijkstra:
                solve_find_cell( &start, nullptr );
                distance = [&]( const coordinate_t& p, const coordinate_t&)
                  {
                    return data[ p.y ][ p.x ] -> distance + 1;
                    //const auto delta = np - start;
                    //return std::abs( delta.x ) + std::abs( delta.y );
                  };
                break;
                
              case solver::algorithm::a_star:
                solve_find_cell( &start, &goal );
                distance = [&]( const coordinate_t&, const coordinate_t& np )
                  {
                    const auto delta = np - goal;
                    return std::abs( delta.x ) + std::abs( delta.y );
                  };
                break;
                
              default:
                throw std::runtime_error("invalid algorithm");
            };
            
            std::priority_queue
              < coordinate_t
              , std::vector< coordinate_t >
              , std::function< auto ( const coordinate_t&, const coordinate_t& ) -> bool >
              >
              search_queue
                ( [&]( const coordinate_t& a, const coordinate_t& b )
                  { return distance( data[ a.y ][ a.x ] -> previous, a ) > distance( data[ b.y ][ b.x ] -> previous, b ); }
                )
              ;
            
            bool path_finded = false;
            
            search = [ & ]( const coordinate_t& p )
            {
              
              for ( const auto& direction : _directions )
              {
                const auto np = p + direction;
                
                if ( np.x < 0
                  or np.y < 0
                  or np.x >= _width
                  or np.y >= _height
                  or not road( (*_maze)[ np.y ][ np.x ] )
                )
                  continue;
                
                auto d = distance( p, np );
                
                auto next_cell = data[ np.y ][ np.x ];
                
                if ( next_cell && next_cell -> distance <= d )
                  continue;
                
                data[ np.y ][ np.x ] = { d, p };
                
                if ( maze::goal( (*_maze)[ np.y ][ np.x ] ) )
                {
                  goal = np;
                  path_finded = true;
                  if ( not _full_gamut )
                    return;
                }
                
                search_queue.push( np );
              }
              
              while( not search_queue.empty() )
              {
                const auto np = search_queue.top();
                search_queue.pop();
                search( np );
                
                if ( path_finded and not _full_gamut )
                  return;
              }
              
            };
            
            data[ start.y ][ start.x ] = { 0, start };
            search( start );
            
            _answer = std::make_shared< answer_t >();
            
            for ( auto cell = goal; cell != start; cell = data[ cell.y ][ cell.x ] -> previous )
              _answer -> emplace_front( std::move( cell ) );
            _answer -> emplace_front( std::move( start ) );
            
            // for debug
            //*
            {
              std::stringstream r;
              for ( const auto data_raw : data )
              {
                for ( const auto data_cell : data_raw )
                  if ( data_cell )
                    r << std::min<char>( '@' + data_cell -> distance, 'Z' );
                  else
                    r << " ";
                r << "\n";
              }
              std::cerr << r.str();
            }
            //*/
            
            return this -> shared_from_this();
          }
          
          auto answer()
            -> shared_answer_t
          { return _answer; }
          
          auto to_string()
            -> std::string
          {
            std::stringstream r;
            for ( size_t n = 0; n < _answer -> size(); ++n )
              r << n << " : ( " << (*_answer)[n].x << ", " << (*_answer)[n].y << " )\n";
            return r.str();
          }
          
        };
      }
    }
  }
}
