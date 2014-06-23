#pragma once

#include <cstdint>
#include <type_traits>
#include <glm/glm.hpp>

namespace wonder_rabbit_project
{
  namespace wonderland
  {
    namespace maze
    {
      struct cell_type
      {
        enum
        { block = 0x00
        , road  = 0x01
        , start = 0x01 | 0x02
        , goal  = 0x01 | 0x04
        };
      };
      
      static inline auto road( std::uint8_t cell )
        -> bool
      { return cell & cell_type::road; }
        
      static inline auto goal( std::uint8_t cell )
        -> bool
      { return cell == cell_type::goal; }
      
      static inline auto start( std::uint8_t cell )
        -> bool
      { return cell == cell_type::start; }
      
      template < class T_coordinate >
      static inline auto generate_directions( )
        -> std::vector< T_coordinate >
      {
        constexpr auto dimension = sizeof( T_coordinate ) / sizeof( typename T_coordinate::value_type );
        std::vector< T_coordinate > r( dimension * 2 );
        
        for ( std::size_t d = 0; d < dimension; ++d )
        {
          r[ d * 2     ][ d ] =  1;
          r[ d * 2 + 1 ][ d ] = -1;
        }
        
        return r;
      }
      
      template< class T_from >
      using to_glm_vec2_t =
        typename std::conditional
        < std::is_same< T_from, std::uint64_t >::value, glm::u64vec2
        , typename std::conditional
          < std::is_same< T_from, std::uint32_t >::value, glm::u32vec2
          , typename std::conditional
            < std::is_same< T_from, std::uint16_t >::value, glm::u16vec2
            , typename std::conditional
              < std::is_same< T_from, std::uint8_t >::value, glm::u8vec2
              , typename std::conditional
                < std::is_same< T_from, std::int64_t >::value, glm::i64vec2
                , typename std::conditional
                  < std::is_same< T_from, std::int32_t >::value, glm::i32vec2
                  , typename std::conditional
                    < std::is_same< T_from, std::int16_t >::value, glm::i16vec2
                    , typename std::conditional
                      < std::is_same< T_from, std::int8_t >::value, glm::i8vec2
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
    
      //template < class T >
      //using voxel_maze_2d_t = std::vector< std::vector< T > >;
      
      namespace generator
      {
        enum class algorithm
        { drill
        };
      }
      
      template < class T = void >
      auto to_string( const generator::algorithm a )
        -> std::string
      {
        switch( a )
        { case generator::algorithm::drill: return "Drill";
          default: throw std::runtime_error( "invalid algorithm." );
        }
      }
      
      namespace solver
      {
        enum class algorithm
        { dijkstra
        , a_star
        };
      }
      
      template < class T = void >
      auto to_string( const solver::algorithm a )
        -> std::string
      {
        switch( a )
        { case solver::algorithm::dijkstra: return "Dijkstra";
          case solver::algorithm::a_star  : return "A*";
          default: throw std::runtime_error( "invalid algorithm." );
        }
      }
      
    }
  }
}