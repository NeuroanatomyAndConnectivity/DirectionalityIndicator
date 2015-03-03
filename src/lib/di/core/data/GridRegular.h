//---------------------------------------------------------------------------------------
//
// Project: DirectionalityIndicator
//
// Copyright 2014-2015 Sebastian Eichelbaum (http://www.sebastian-eichelbaum.de)
//           2014-2015 Max Planck Research Group "Neuroanatomy and Connectivity"
//
// This file is part of DirectionalityIndicator.
//
// DirectionalityIndicator is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DirectionalityIndicator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DirectionalityIndicator. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#ifndef DI_GRIDREGULAR_H
#define DI_GRIDREGULAR_H

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include <di/core/BoundingBox.h>
#include <di/core/data/GridTransformation.h>

#include <di/MathTypes.h> // needed for storing physical bounds

#include <di/core/Logger.h>
#define LogTag "core/GridRegular"

namespace di
{
    namespace core
    {
        /**
         * Some implementation details.
         */
        namespace detail
        {
            /**
             * Implement the sequence pattern to "inflate" arrays to variadic function calls.
             *
             * \tparam Is index sequence
             */
            template<int... Is>
            struct seq { };

            /**
             * Sequence generator. Recursive template to generate the \ref seq sequence.
             *
             * \tparam N how many indices to create?
             * \tparam Is The current set of indices.
             */
            template<int N, int... Is>
            struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

            /**
             * Sequence generator. Recursive template to generate the \ref seq sequence. Stop recursion.
             *
             * \tparam Is The current set of indices.
             */
            template<int... Is>
            struct gen_seq<0, Is...> : seq<Is...> { };
        }

        /**
         * Implementation of an standard regular grid of arbitrary dimension. The class mainly serves as a "indexer" to the linear data layout in the
         * value stores used in the \ref DataSet class.
         */
        template< size_t NumberOfDimensions = 3, typename IndexType = size_t >
        class GridRegular
        {
        public:
            /**
             * The number of dimensions of this grid.
             */
            const static size_t Dimensions = NumberOfDimensions;

            /**
             * The type used for indices.
             */
            typedef IndexType value_type;

            /**
             * The type of a grid sliced (reduced by one dimension).
             */
            typedef GridRegular< NumberOfDimensions - 1, IndexType > SlicedGridType;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Construction
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Construct a grid using the given data. If too few parameters in voxelsPerDim where given, remaining dimensions will be assumed to be 1
             * voxel wide.
             *
             * \param voxelsPerDim the number of voxels in each direction.
             * \param transform the transformation of this grid into the world space.
             */
            explicit GridRegular( const GridTransformation< Dimensions >& transform, const std::initializer_list< IndexType >& voxelsPerDim ):
                m_transform( transform )
            {
                // Copy the given arguments and use 1 if the user specified too few arguments
                typename std::initializer_list< IndexType >::const_iterator argIt = voxelsPerDim.begin();
                for( IndexType i = 0; i < Dimensions; ++i )
                {
                    if( argIt != voxelsPerDim.end() )
                    {
                        m_sizes[ i ] = *argIt;
                    }
                    else
                    {
                        m_sizes[ i ] = 1;
                    }
                    ++argIt;
                }
            }

            /**
             * Construct a grid using the given voxel dimensions
             *
             * \param voxels the number of voxels in each direction
             * \param transform the grid transformation describing the orientation in space.
             */
            explicit GridRegular( const GridTransformation< Dimensions >& transform, const std::array< IndexType, Dimensions >& voxels ):
                m_transform( transform )
            {
                // NOTE: converting the array into an initializer_list is not possible directly.

                // Copy the given arguments and use 1 if the user specified too few arguments
                typename std::array< IndexType, Dimensions >::const_iterator argIt = voxels.begin();
                for( IndexType i = 0; i < Dimensions; ++i )
                {
                    if( argIt != voxels.end() )
                    {
                        m_sizes[ i ] = *argIt;
                    }
                    else
                    {
                        m_sizes[ i ] = 1;
                    }
                    ++argIt;
                }
            }

            /**
             * Construct a grid using the given voxel dimensions.
             *
             * \param voxelsX the number of voxels in the 1st direction
             * \param voxelsY the number of voxels in the 2nd direction
             * \param voxelsZ the number of voxels in the 3rd direction
             * \param voxelsT the number of voxels in the 4th direction
             * \param transform the grid transformation describing the orientation in space.
             */
            GridRegular( const GridTransformation< Dimensions >& transform, const IndexType& voxelsX, const IndexType& voxelsY = 1,
                         const IndexType& voxelsZ = 1, const IndexType& voxelsT = 1 ):
                GridRegular( transform, std::initializer_list< IndexType >( { voxelsX, voxelsY, voxelsZ, voxelsT } ) )
            {
            }

            /**
             * Construct a grid using the given voxel dimensions
             *
             * \param voxels the number of voxels in each direction
             * \param transform the grid transformation describing the orientation in space.
             */
            explicit GridRegular( const GridTransformation< Dimensions >& transform, const glm::ivec4& voxels ):
                GridRegular( transform, voxels.x, voxels.y, voxels.z, voxels.w  )
            {
            }

            /**
             * Construct a grid using the given voxel dimensions
             *
             * \param voxels the number of voxels in each direction
             * \param transform the grid transformation describing the orientation in space.
             */
            explicit GridRegular( const GridTransformation< Dimensions >& transform, const glm::ivec3& voxels ):
                GridRegular( transform, voxels.x, voxels.y, voxels.z )
            {
            }

            /**
             * Construct a grid using the given voxel dimensions
             *
             * \param voxels the number of voxels in each direction
             * \param transform the grid transformation describing the orientation in space.
             */
            explicit GridRegular( const GridTransformation< Dimensions >& transform, const glm::ivec2& voxels ):
                GridRegular( transform, voxels.x, voxels.y )
            {
            }

            /**
             * Copy the given grid to a new one.
             *
             * \param other the grid to copy
             */
            GridRegular( const GridRegular& other ):
                m_sizes( other.m_sizes ),
                m_transform( other.transform )
            {
                // nothing more to do.
            }

            /**
             * Destructor. Clean up and free the memory.
             */
            ~GridRegular() noexcept
            {
                // nothing more to do.
            }

            /**
             * Copy assignment. Assign another GridRegular to this one by copying it.
             *
             * \param other the other GridRegular to assign to this one.
             *
             * \return this
             */
            GridRegular& operator=( const GridRegular& other )
            {
                m_sizes = other.m_sizes;
                m_transform = other.m_transform;
                return *this;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Property Query
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Query the number of dimensions of this grid.
             *
             * \return the number of dimensions
             */
            IndexType getDimensions() const
            {
                return Dimensions;
            }

            /**
             * Get the size of the grid accumulating all dimensions. In other words, how many voxels are inside the grid.
             *
             * \return the number of voxels in the grid
             */
            IndexType getSize() const
            {
                return accumulateSizes( Dimensions );
            }

            /**
             * Get the size of the grid along the given axis. If the queried dimension does not exist in this grid, 1 is returned as every 2D grid is
             * a cube with a depth of only 1 voxel and so on.
             *
             * \param dimension the axis (dimension) to query.
             *
             * \return the number of voxels in this direction
             */
            IndexType getSize( IndexType dimension ) const
            {
                if( dimension >= getDimensions() )
                {
                    return 1;
                }
                return m_sizes[ dimension ];
            }

            // Provide some default queries for 2d, 3d, and 4d grid for convenience.

            /**
             * Return the number of voxels in X direction.
             *
             * \return the number of voxels
             */
            IndexType getSizeX() const
            {
                return getSize( 0 );
            }

            /**
             * Return the number of voxels in X direction.
             *
             * \return the number of voxels
             */
            IndexType getSizeY() const
            {
                return getSize( 1 );
            }

            /**
             * Return the number of voxels in X direction.
             *
             * \return the number of voxels
             */
            IndexType getSizeZ() const
            {
                return getSize( 2 );
            }

            /**
             * Return the number of voxels in X direction.
             *
             * \return the number of voxels
             */
            IndexType getSizeT() const
            {
                return getSize( 3 );
            }

            /**
             * Get an array with all voxel numbers of all dimensions.
             *
             * \return the number of voxels for each dimension.
             */
            std::array< IndexType, Dimensions > getSizes() const
            {
                return m_sizes;
            }

            /**
             * Query the transform of this grid.
             *
             * \return the transform
             */
            const GridTransformation< Dimensions >& getTransformation() const
            {
                return m_transform;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //
            // Indexing
            //
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            /**
             * Get the index of the specified voxel. The coordinates used here are in grid-space, NOT in data-space. You can define an arbitrary
             * amount of coordinates here, at least 1. All the other coordinates missing are assumed to be 0. If specifying too much coordinates, they
             * are ignored.
             *
             * \note this is a recursive template function.
             *
             * \tparam dim the dimension currently handled.
             * \param coords current coordinate
             *
             * \throw std::out_of_range if a coordinate is too large.
             *
             * \return the index in a linear memory
             */
            template< IndexType dim = 0 >
            IndexType index( IndexType coords ) const
            {
                // handle d) directly
                if( coords >= m_sizes[ dim ] )
                {
                    throw std::out_of_range( "Coordinate " + std::to_string( dim ) + " is too large: " + std::to_string( coords ) + ">=" +
                                             std::to_string( m_sizes[ dim ] ) + "." );
                }

                // NOTE: the case b) mentioned below is handled implicitly.
                return accumulateSizes( dim ) * coords;
            }

            /**
             * Get the index of the specified voxel. The coordinates used here are in grid-space, NOT in data-space. You can define an arbitrary
             * amount of coordinates here, at least 1. All the other coordinates missing are assumed to be 0. If specifying too much coordinates, they
             * are ignored.
             *
             * \note this is a recursive template function.
             *
             * \tparam dim the dimension currently handled.
             * \tparam NextCoords the remaining coordinates not yet handled.
             * \param coords current coordinate
             * \param nextCoords the remaining coordinate not yet handled.
             *
             * \throw std::out_of_range if a coordinate is too large.
             *
             * \return the index in a linear memory
             */
            template< IndexType dim = 0, typename... NextCoords >
            IndexType index( IndexType coords, NextCoords... nextCoords ) const
            {
                // Memory index is calculated as follows
                //
                // 1D "Cube":
                // memIndex = coord0;
                //
                // 2D "Cube":
                // memIndex = ( size0 * coord1 ) + coord0;
                //
                // 3D "Cube":
                // memIndex = ( size0 * size1 * coord2 ) + ( size0 * coord1 ) + coord0;
                //
                // ..... and so on. This yields this recursive template:

                // The template variable "dim" denotes which coordinate we are processing. According to the above example, we need to
                // multiply all previous slices in the data up to dim-1. The parameter c is the coordinate in dimension "dim"

                // Now follows the C++ part.
                //
                // The following scenarios occur:
                //
                // a) the user specifies the exactly needed amount of coordinates. Everything is nice.
                // b) the user specifies less -> assume all other coordinates to be 0.
                // c) the user specifies more coordinates than needed -> assume all other coordinates to be 0.
                // d) a coordinate is too huge

                // handle d) directly
                if( coords >= m_sizes[ dim ] )
                {
                    throw std::out_of_range( "Coordinate " + std::to_string( dim ) + " is too large: " + std::to_string( coords ) + ">=" +
                                             std::to_string( m_sizes[ dim ] ) + "." );
                }

                // c) As we work recursively, we need to define an end IF the user specified more arguments than dimensions.
                if( dim + 1 == getDimensions() )
                {
                    // all the remaining coordinates in "nextCoords" are assumed to be 0 -> do not influence offset anymore
                    return accumulateSizes( dim ) * coords;
                }

                // Accumulate all previous sizes ...
                // ... and multiply this offset with the current coord and add to the offset of the next dimensions.
                return index< dim + 1 >( nextCoords... ) + ( accumulateSizes( dim ) * coords );
            }

            /**
             * \copydoc index
             *
             * \tparam dims size of the input array
             * \tparam Is the indices up to "dims"
             * \param coords coord array
             *
             * \return the index.
             */
            template< size_t dims, int... Is >
            IndexType index( const std::array< IndexType, dims >& coords, detail::seq< Is... > /* seq */ ) const
            {
                return index( coords[Is]... );
            }

            /**
             * \copydoc index
             *
             * \tparam dims size of the input array
             * \param coords the coords array.
             *
             * \return the index
             */
            template< size_t dims >
            IndexType index( std::array< IndexType, dims > coords ) const
            {
                return index( coords, detail::gen_seq< dims >() );
            }

            /**
             * \copydoc index
             *
             * \param coords the coords array.
             *
             * \return the index
             */
            IndexType index( glm::ivec3 coords ) const
            {
                return index( coords.x, coords.y, coords.z );
            }

            /**
             * \copydoc index
             *
             * \param coords the coords array.
             *
             * \return the index
             */
            IndexType index( glm::ivec4 coords ) const
            {
                return index( coords.x, coords.y, coords.z, coords.w );
            }

            /**
             * Get the index of the voxel where the point specified is in.
             *
             * \param v the coordinate of the point inside the voxel to find. Defined in world-space.
             *
             * \return the index of the voxel
             */
            IndexType voxelIndex( glm::vec3 v ) const
            {
                auto g = m_transform * v;

                // Due to numeric issues, we ensure the coordinate is never below 0
                IndexType x = std::floor( g.x ) < 0.0 ? 0 : static_cast< IndexType >( g.x );
                IndexType y = std::floor( g.y ) < 0.0 ? 0 : static_cast< IndexType >( g.y );
                IndexType z = std::floor( g.z ) < 0.0 ? 0 : static_cast< IndexType >( g.z );

                return index( x, y, z );
            }

        protected:
        private:
            /**
             * Multiply all items in m_sizes up to but not including the given dimension.
             *
             * \param upToButNotIncluding the dimension to stop accumulation
             *
             * \return the multiplied sizes
             */
            IndexType accumulateSizes( IndexType upToButNotIncluding ) const
            {
                IndexType ac = std::accumulate( m_sizes.begin(), m_sizes.begin() + upToButNotIncluding, static_cast< IndexType >( 1 ),
                                                [](int i, int j) { return i * j; } // multiply
                               );
                return ac;
            }

            /**
             * Number if voxels per dimension.
             */
            std::array< IndexType, Dimensions > m_sizes;

            /**
             * Transformation of the grid.
             */
            GridTransformation< Dimensions > m_transform;
        };

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Abbreviations
        //
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Time-dependent 3D field or time-independent 4D field.
        typedef GridRegular< 4 > GridRegular4;
        // Time-dependent 2D field or time-independent 3D field.
        typedef GridRegular< 3 > GridRegular3;
        // Time-dependent 1D field or time-independent 2D field.
        typedef GridRegular< 2 > GridRegular2;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // Operators
        //
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * Print the object to a stream. Used for pretty-printing.
         *
         * \param os the stream to print to
         * \param obj the instance
         *
         * \return os.
         */
        template< size_t Dimensions >
        std::ostream& operator<<( std::ostream& os, const GridRegular< Dimensions >& obj )
        {
            os << "Grid information: Dimensions=" << Dimensions << ", Resolution: [";
            for( auto s : obj.getSizes() )
            {
                os << s << ", ";
            }
            os << "].";
            return os;
        }
    }
}

#endif  // DI_GRIDREGULAR_H

