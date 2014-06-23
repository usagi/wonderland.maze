#pragma once

#include <string>
#include <memory>
#include <random>
#include <type_traits>
#include <sstream>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>

#include "traits.hxx"

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace maze
    {
      namespace generator
      {
        template
        < class T_rng  = std::conditional< sizeof( void* ) == 8, std::mt19937_64  , std::mt19937      >::type
        , class T_size = std::int_fast32_t
        >
        class generator_2d_t
          : public std::enable_shared_from_this
            < generator_2d_t
              < T_rng
              , T_size
              >
            >
        {
        public:
          using rng_t  = T_rng;
          using size_t = T_size;
          
          using shared_t = std::shared_ptr< generator_2d_t >;
          
          static constexpr auto dimension = std::size_t( 2 );
          
          using coordinate_t = to_glm_vec2_t< size_t >;
          
          using data_t = std::vector< std::vector< std::uint8_t > >;
          using shared_data_t = std::shared_ptr< data_t >;
          
          static constexpr auto default_size = size_t( 13 );
          
        private:
          size_t _width  = default_size;
          size_t _height = default_size;
          
          shared_data_t _maze;
          
          std::shared_ptr< rng_t > _rng;
          
          const std::vector< coordinate_t > _directions;
          
          std::string _block_cell_string
                    , _road_cell_string 
                    , _unknown_cell_string
                    , _start_cell_string
                    , _goal_cell_string
                    ;
          
          coordinate_t _far_cell;
          size_t       _far_distance;
          
          generator::algorithm _algorithm;
          
          template < class T >
          static inline auto odd( T value )
            -> bool
          { return value % 2; }
          
          template < class T >
          static inline auto exception_if_not_odd( T value )
            -> void
          {
            if ( not odd( value ) )
              throw std::runtime_error( "value is not odd." );
          }
          
          template < class T = size_t >
          auto generate_random_odd( const T max )
            -> T
          {
            std::uniform_int_distribution< T > dist( 0, ( max >> 1 ) - 1 );
            return dist( *_rng ) * 2 + 1;
          }
          
          auto generate_random_directions_indices( )
            -> std::vector< std::uint_fast8_t >
          {
            std::vector< std::uint_fast8_t > r( dimension * 2 );
            std::uniform_int_distribution< std::uint_fast8_t > dist( 0, dimension * 2 - 1 );
            std::iota( std::begin( r ), std::end( r ), 0 );
            std::shuffle( std::begin( r ), std::end( r ), *_rng );
            return r;
          }
          
          auto drill( const coordinate_t& p, std::size_t distance = 0 )
            -> void
          {
            if ( distance > _far_distance )
            {
              _far_cell = p;
              _far_distance = distance;
            }
            
            const auto indices = generate_random_directions_indices();
            
            for ( const auto i : indices )
            {
              const auto& d = _directions[ i ];
              
              const auto q1 = p + d;
              const auto q2 = q1 + d;
              
              if ( q2.x < 0 or q2.x >= _width or q2.y < 0 or q2.y >= _height )
                continue;
              
              if ( not road( (*_maze)[ q2.y ][ q2.x ] ) )
              {
                (*_maze)[ q1.y ][ q1.x ] = cell_type::road;
                (*_maze)[ q2.y ][ q2.x ] = cell_type::road;
                
                drill( q2, distance + 2 );
              }
            }
          }
          
        public:
          
          generator_2d_t()
            : _rng ( std::make_shared< rng_t >( ) )
            , _directions( generate_directions< coordinate_t >() )
            , _block_cell_string   ( std::to_string( cell_type::block ) )
            , _road_cell_string    ( std::to_string( cell_type::road  ) )
            , _unknown_cell_string ( u8"?" )
            , _start_cell_string   ( u8"S" )
            , _goal_cell_string    ( u8"G" )
            , _algorithm( generator::algorithm::drill )
          { }
          
          auto algorithm( const generator::algorithm a )
            -> shared_t
          {
            _algorithm = a;
            return this ->shared_from_this();
          }
          
          auto size( const size_t size_ )
            -> shared_t
          { return this -> width( size_ ) -> height( size_ ); }
          
          auto width( const size_t width_ )
            -> shared_t
          {
            exception_if_not_odd( width_ );
            _width = width_;
            return this -> shared_from_this();
          }
          
          auto height( const size_t height_ )
            -> shared_t
          {
            exception_if_not_odd( height_ );
            _height = height_;
            return this -> shared_from_this();
          }
          
          auto block_cell_string( const std::string s )
            -> shared_t
          {
            _block_cell_string = s;
            return this -> shared_from_this();
          }
          
          auto road_cell_string( const std::string s )
            -> shared_t
          {
            _road_cell_string = s;
            return this -> shared_from_this();
          }
          
          auto unknown_cell_string( const std::string s )
            -> shared_t
          {
            _unknown_cell_string = s;
            return this -> shared_from_this();
          }
          
          auto start_cell_string( const std::string s )
            -> shared_t
          {
            _start_cell_string = s;
            return this -> shared_from_this();
          }
          
          auto goal_cell_string( const std::string s )
            -> shared_t
          {
            _goal_cell_string = s;
            return this -> shared_from_this();
          }
          
          auto rng( const std::shared_ptr< rng_t >& rng )
            -> shared_t
          {
            _rng = rng;
            return this -> shared_from_this();
          }
          
          auto generate( )
            -> shared_t
          {
            _maze = std::make_shared< data_t >( _height, std::vector< std::uint8_t >( _width, cell_type::block ) );
            
            const coordinate_t goal
              { generate_random_odd( _width  )
              , generate_random_odd( _height )
              };
            
            (*_maze)[ goal.y ][ goal.x ] = cell_type::goal;
            
            _far_distance = 0;
            _far_cell = goal;
            
            drill( goal );
            
            (*_maze)[ _far_cell.y ][ _far_cell.x ] = cell_type::start;
            
            return this -> shared_from_this();
          }
          
          auto to_string( ) const
            -> std::string
          {
            std::stringstream r;
            for ( const auto maze_raw : *_maze )
            {
              for ( const auto maze_cell : maze_raw )
                switch ( maze_cell )
                { case cell_type::block: r << _block_cell_string;   break;
                  case cell_type::road : r << _road_cell_string;    break;
                  case cell_type::start: r << _start_cell_string;   break;
                  case cell_type::goal : r << _goal_cell_string;    break;
                  default              : r << _unknown_cell_string;
                }
              r << "\n";
            }
            return r.str();
          }
          
          auto data( )
            -> shared_data_t
          { return _maze; }
        };
      }
    }
  }
}