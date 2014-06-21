#pragma once

#include <string>
#include <memory>
#include <random>
#include <type_traits>
#include <sstream>
#include <algorithm>
#include <vector>

#include <glm/glm.hpp>

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace maze
    {

      template
      < class T_rng  = std::conditional< sizeof( void* ) == 8, std::mt19937_64  , std::mt19937      >::type
      , class T_size = std::int_fast32_t
      >
      class generator_drill_2d_t
        : public std::enable_shared_from_this
          < generator_drill_2d_t
            < T_rng
            , T_size
            >
          >
      {
      public:
        using rng_t  = T_rng;
        using size_t = T_size;
        
        using shared_t = std::shared_ptr< generator_drill_2d_t >;
        
        static constexpr auto dimension = std::size_t( 2 );
        
        using coordinate_t =
          typename std::conditional
          < std::is_same< size_t, std::uint64_t >::value, glm::u64vec2
          , typename std::conditional
            < std::is_same< size_t, std::uint32_t >::value, glm::u32vec2
            , typename std::conditional
              < std::is_same< size_t, std::uint16_t >::value, glm::u16vec2
              , typename std::conditional
                < std::is_same< size_t, std::uint8_t >::value, glm::u8vec2
                , typename std::conditional
                  < std::is_same< size_t, std::int64_t >::value, glm::i64vec2
                  , typename std::conditional
                    < std::is_same< size_t, std::int32_t >::value, glm::i32vec2
                    , typename std::conditional
                      < std::is_same< size_t, std::int16_t >::value, glm::i16vec2
                      , typename std::conditional
                        < std::is_same< size_t, std::int8_t >::value, glm::i8vec2
                        , void
                        >::type
                      >::type
                    >::type
                  >::type
                >::type
              >::type
            >::type
          >::type
          ;
        
        static constexpr auto default_size = size_t( 13 );
        
        struct cell_type
        {
          enum
          { block = 0x00
          , road  = 0x01
          , start = 0x01 | 0x02
          , goal  = 0x01 | 0x04
          };
        };
        
      private:
        size_t _width  = default_size;
        size_t _height = default_size;
        
        std::vector< std::vector < std::uint8_t > > _maze;
        
        std::shared_ptr< rng_t > _rng;
        
        std::string _block_cell_string
                  , _road_cell_string 
                  , _unknown_cell_string
                  , _start_cell_string
                  , _goal_cell_string
                  ;
        
        coordinate_t _far_cell;
        size_t       _far_distance;
        
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
        
        static inline auto road( std::uint8_t cell )
          -> bool
        { return cell & cell_type::road; }
        
        template < class T = size_t >
        auto generate_random_odd( const T max )
          -> T
        {
          std::uniform_int_distribution< T > dist( 0, ( max >> 1 ) - 1 );
          return dist( *_rng ) * 2 + 1;
        }
        
        auto generate_directions( )
          -> std::vector< coordinate_t >
        {
          return
          { { {  1 ,  0 }
            , { -1 ,  0 }
            , {  0 ,  1 }
            , {  0 , -1 }
            }
          };
        }
        
        auto generate_random_directions( )
          -> std::vector< coordinate_t >
        {
          auto ds = generate_directions();
          std::shuffle( std::begin( ds ), std::end( ds ), *_rng );
          return ds;
        }
        
        auto drill( const coordinate_t& p, std::size_t distance = 0 )
          -> void
        {
          if ( distance > _far_distance )
          {
            _far_cell = p;
            _far_distance = distance;
          }
          
          for ( const auto d : generate_random_directions() )
          {
            const auto q1 = p + d;
            const auto q2 = q1 + d;
            
            if ( q2.x < 0 or q2.x >= _width or q2.y < 0 or q2.y >= _height )
              continue;
            
            if ( not road( _maze[ q2.y ][ q2.x ] ) )
            {
              _maze[ q1.y ][ q1.x ] = cell_type::road;
              _maze[ q2.y ][ q2.x ] = cell_type::road;
              
              drill( q2, distance + 2 );
            }
          }
        }
        
      public:
        
        generator_drill_2d_t()
          : _rng ( std::make_shared< rng_t >( ) )
          , _block_cell_string   ( std::to_string( cell_type::block ) )
          , _road_cell_string    ( std::to_string( cell_type::road  ) )
          , _unknown_cell_string ( u8"?"                              )
          , _start_cell_string   ( u8"S" )
          , _goal_cell_string    ( u8"G" )
        { }
        
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
          _maze.resize( _height, std::vector< std::uint8_t >( _width, cell_type::block ) );
          
          const coordinate_t goal
            { generate_random_odd( _width  )
            , generate_random_odd( _height )
            };
          
          _maze[ goal.y ][ goal.x ] = cell_type::goal;
          
          _far_distance = 0;
          _far_cell = goal;
          
          drill( goal );
          
          _maze[ _far_cell.y ][ _far_cell.x ] = cell_type::start;
          
          return this -> shared_from_this();
        }
        
        auto to_string( ) const
          -> std::string
        {
          std::stringstream r;
          for ( const auto maze_raw : _maze )
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
      };
    }
  }
}