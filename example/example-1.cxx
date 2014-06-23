#include <iostream>

#include <wonder_rabbit_project/wonderland/maze.hxx>

int main( int number_of_parameters, char** parameters )
{
  using namespace wonder_rabbit_project::wonderland;
  
  auto g = std::make_shared< maze::generator::generator_2d_t<> >();
  auto s = std::make_shared< maze::solver::solver_2d_t<> >();
  
  switch ( number_of_parameters )
  {
    case 8:
      g -> goal_cell_string( parameters[7] );
    case 7:
      g -> start_cell_string( parameters[6] );
    case 6:
      g -> unknown_cell_string( parameters[5] );
    case 5:
      g -> road_cell_string( parameters[4] );
    case 4:
      g -> block_cell_string( parameters[3] );
    case 3:
      g -> height( std::stoull( parameters[2] ) );
    case 2:
      g -> width( std::stoull( parameters[1] ) );
  }

#ifdef EMSCRIPTEN
  // in Emscripten-1.20.0 can not use std::random_device default ctor.
  // https://github.com/kripken/emscripten/issues/2439
  auto rng = std::make_shared< decltype( g )::element_type::rng_t >( );
#else
  auto rng = std::make_shared< decltype( g )::element_type::rng_t >(4 /*std::random_device()()*/ );
#endif
  
  constexpr auto generator_algorithm = maze::generator::algorithm::drill;
  constexpr auto solver_algorithm    = maze::solver::algorithm::a_star;
  
  std::cout
    << "\n"
       "[ generate ( " << maze::to_string( generator_algorithm ) << " ) ]\n\n"
    << g -> rng( rng )
         -> algorithm( generator_algorithm )
         -> generate()
         -> to_string()
    << "\n"
       "[ solve ( " << maze::to_string( solver_algorithm ) << " ) ]\n\n"
    << s -> load( g -> data() )
         -> algorithm( solver_algorithm )
         -> solve()
         -> to_string()
    ;
  
}
