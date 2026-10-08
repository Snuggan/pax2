//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"
#include <format>


namespace pax {
	
	/// Implements an object with two corner coordinates, a bounding box.
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used as a simple bounding box, to check if points or circles/plots are inside. 
	/// It is a superclass for Box_indexer, below, a tool to convert coordinates to a pixel in a raster.
	template< floating F, std::size_t N >							requires( is_static< N > )
	struct Box {
		static constexpr std::size_t 		rank				  = N;
		using 								Pt					  = Point< F, N >;
		using 								Base				  = std::array< Pt, 2 >;
		using 								value_type			  = Pt::value_type;

	private:
		std::array< Pt, 2 >					m_box{};
		
	public:
		constexpr Box()											  = default;
		constexpr Box( const Box & )							  = default;
		constexpr Box & operator=( const Box & )				  = default;

		constexpr Box( const Pt & pt0_, const Pt & pt1_ )			noexcept 
			: m_box({ pax::min( pt0_, pt1_ ), pax::max( pt0_, pt1_ ) }) {}

		constexpr const Base & box()								const noexcept	{	return m_box;					}
		constexpr const Pt   & min()								const noexcept	{	return box().front();			}
		constexpr const Pt   & max()								const noexcept	{	return box().back();			}
		constexpr       bool   empty() 								const noexcept	{	return !all_lt( min(), max() );	}
		constexpr       Pt     sides()								const noexcept	{	return max() - min();			}

		friend constexpr const Pt & min( const Box & b_ )			noexcept		{	return b_.min();				}
		friend constexpr const Pt & max( const Box & b_ )			noexcept		{	return b_.max();				}
		friend constexpr bool     empty( const Box & b_ )			noexcept		{	return b_.empty();				}

		friend constexpr bool operator==( const Box & b0_, const Box & b1_ ) noexcept {
			return ( b0_.min() == b1_.min() ) && ( b0_.max() == b1_.max() );
		}

		/// Returns a Box that contains *this and is evenly divisable by corresponding elements in resolution_.
		constexpr Box aligned( const Pt resolution_ )				const noexcept	{
			static_assert( floating< F >, "The algorithms below doesn't work with integers." );
			static constexpr auto align_le = []( const F v_, const F res_ ) {
				return res_ ? res_ * std::floor( v_ / res_ ) : v_;
			};
			static constexpr auto align_ge = []( const F v_, const F res_ ) {
				return res_ ? res_ * std::ceil ( v_ / res_ ) : v_;
			};

			const auto [ ...   res ]	  = resolution_;
			const auto [ ... small ]	  = min();
			const auto [ ... large ]	  = max();
			return { { align_le( small, std::abs( res ) ) ... }, 	// The value <= v_ evenly divisible by res_.
					 { align_ge( large, std::abs( res ) ) ... } };	// The value >= v_ evenly divisible by res_.
		}

		/// Returns a Box that contains *this and is evenly divisable by [scalar] resolution_.
		constexpr Box aligned( const value_type resolution_ )		const noexcept	{
			return aligned( pax::point< rank >( resolution_ ) );
		}

		/// Returns the minimal Box that contains both the original Box and pt_.
		constexpr Box grow( const Pt & pt_ )						const noexcept	{
			return { pax::min( min(), pt_ ), pax::max( max(), pt_ ) };
		}

		/// Is the point inside the Box but not on its borders?
		constexpr bool strictly_inside( const Pt & pt_ )			const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		/// Is the point inside the Box or on its borders?
		constexpr bool inside_or_on( const Pt & pt_ )				const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Is the point inside the Box or on its minimal (but not maximal) borders?
		constexpr bool in_range( const Pt & pt_ )					const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}

		/// Box contents as a std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{}", m_box );
		}
	};
	
	using Box2d							  = Box< double, 2 >;
	using Box3d							  = Box< double, 3 >;

	template< floating F, std::size_t N >
	Box( const Point< F, N > &, const Point< F, N > & ) -> Box< F, N >;
	
	template< typename Out, floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Box< F, N >				  & box_
	) {	return out_ << std::string( box_ );					}




	/// Handles transformation of multiple indices (of matrix) to scalar index [into a vector]. 
	/// It can have any rank you please, but two (and sometimes three) is probably the usual.
	/// It may be used to handle multiple indeces, such as for rasters and multi-dimensional arrays.
	/// It is a superclass for Box_indexer, below, together with Box.
	template< std::size_t N >										requires( is_static< N > )
	struct Indexer {
		static constexpr std::size_t		rank				  = N;
		using 								Idx					  = Index< rank >;
		using 								index_type			  = Idx::value_type;

	private:
		Idx									m_extents{}, m_offsets{};
		static constexpr Idx				noll{};
		
		static constexpr Idx calculate_offsets( const Idx & idx_ )	noexcept		{
			index_type						product{ 1u };
			auto [ ... t, tn ]			  = idx_;
			return { product, ( product *= t ) ... };
		}
		
	public:
		constexpr Indexer()										  = default;
		constexpr Indexer( const Indexer & )					  = default;
		constexpr Indexer & operator=( const Indexer & )		  = default;

		constexpr Indexer( const Idx & extents_ )  					noexcept 
			: m_extents( extents_ ), m_offsets{ calculate_offsets( extents_ ) } {}

		/// Number of elementa in each dimension.
		constexpr const Idx & extents()								const noexcept	{	return m_extents;			}

		/// The stride between elements in each dimension. 
		constexpr const Idx & offsets()								const noexcept	{	return m_offsets;			}
		
		/// The number of "columns", same as size()[ col_idx ].
		constexpr index_type cols()									const noexcept	{	return col( extents() );	}
		
		/// The number of "rows", same as size()[ row_idx ].
		constexpr index_type rows()									const noexcept	{	return row( extents() );	}

		/// The total number of elements (product of all sizes).
		constexpr index_type elements()								const noexcept	{
			return offsets().back()*extents().back();
		}
		
		/// There are no elements (elements() == 0).
		constexpr bool empty()										const noexcept	{
			return !offsets().back() || !extents().back();
		}
		
		/// Returns true, iff all indeces in i_ are smaller the the eqivalent size. 
		constexpr bool valid_index( const Idx & idx_ )				const noexcept	{
			return all_lt( idx_, extents() );
		}
		
		/// Calculate the index into a vector for the index represented by u_.
		template< uinteger ...U >									requires( sizeof...( U ) == N )
		constexpr index_type scalar_index( U && ... u_ )			const noexcept	{
			return scalar_index( Idx{ std::forward< U >( u_ ) ... } );
		}
		
		/// Calculate the index into a vector for the index represented by idx_.
		constexpr index_type scalar_index( const Idx & idx_ )		const noexcept	{
			return dot_product( idx_, offsets() );
		}

		/// Indexer contents as a std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{}", extents() );
		}
	};
	
	using Indexer2d						  = Indexer< 2 >;
	using Indexer3d						  = Indexer< 3 >;

	template< std::size_t N >
	Indexer( const Index< N > & ) -> Indexer< N >;
	
	template< typename Out, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Indexer< N >				  & idxer_
	) {	return out_ << std::string( idxer_ );			}




	/// A bounding box that also handles coordinattes to scalar index transformation.
	/// - The bounding box always has corners aligned with the resolution. 
	/// - It will reverse the direction of those axis with negative resolution. 
	/// - If you intend to use it with a [gdal] raster or pictures, you should probably give a
	///   positove x-resolution and a negative y-resolution as they usually have upper left as 
	///   origo and go "down".
	template< floating F, std::size_t N >						requires( is_static< N > )
	struct Box_indexer : public Box< F, N >, public Indexer< N > {
		static constexpr std::size_t 		rank			  = N;
		using 								BBox			  = Box< F, rank >;
		using 								Pt				  = BBox::Pt;
		using 								value_type		  = Pt::value_type;
		using 								Idx				  = Indexer< rank >;
		using 								index_type		  = Idx::index_type;
		using 								BBox::min, BBox::max;

	protected:
		Pt									m_resolution{};		// The element size (all positive).
		Pt									m_factor{};			// Multiply a point with this...
		Pt									m_offset{};			// ...and add this to get the index. 

		static constexpr Point< std::size_t, N > calculate_extents(
			const Pt					  & sides_,
			const Pt					  & resolution_
		) noexcept {
			// We want no zero-length dimension.
			static constexpr auto mini	  = []( index_type i_ ) { return ( i_ > 1u ) ? i_ : 1u; };
			const auto [ ... side ]		  = sides_;
			const auto [ ...  res ]		  = resolution_;
			return { mini( side/std::abs( res ) ) ... };
		}
		
		static constexpr auto smallest = []( value_type c, index_type i ){
			const auto ci = static_cast< index_type >( c );
			return ( ci < i ) ? ci : i;
		};


	public:
		using coord_type										  = value_type;

		constexpr Box_indexer()									  = default;
		constexpr Box_indexer( const Box_indexer & )			  = default;
		constexpr Box_indexer & operator=( const Box_indexer & )  = default;

		/// The main constructor that calculates the transformation attributes.
		/// Note: The bbox cormers will be aligned with the resolution, so might slightly differ from box_.
		///       The aligned bbow will be equal to or larger than that defined by box_.
		constexpr Box_indexer(
			const BBox					  & box_, 
			const Pt					  & resolution_
		) : 
			BBox{ box_.aligned( resolution_ ) }, 
			Idx { calculate_extents( BBox::sides(), resolution_ ) }, 
			m_resolution( resolution_ )
		{
			// Calculate the actual transformation attributes. 
			const auto [ ... min ]		  = BBox::min();
			const auto [ ... max ]		  = BBox::max();
			const auto [ ... res ]		  = resolution_;
			if( ( ( res == 0 ) || ... ) ) throw std::runtime_error( 
				std::format( "No resolution element may be zero, but they are: {}.", resolution_ ) );

			m_factor					  = { 1/res ... };
			m_offset					  = { ( ( res > 0 ) ? min : max )/-res ... };
		}

		/// Simplified constructor, when elements have the same length in all dimensions.
		/// Note: The bbox cormers will be aligned with the resolution, so might slightly differ from box_.
		///       The aligned bbow will be equal to or larger than that defined by box_.
		constexpr Box_indexer(
			const BBox					  & box_, 
			const value_type				resolution_
		) : Box_indexer( box_, pax::point< rank >( resolution_ ) ) {}

		/// Simplified constructor, when elements have the same length in all dimensions.
		/// - Note: The bbox cormers will be aligned with the resolution, so might slightly differ from values in aff_.
		///         The aligned bbow will be equal to or larger than that defined by aff_.
		/// east  = aff_[0] + col*aff_[1] + row*aff_[2];
		/// north = aff_[3] + col*aff_[4] + row*aff_[5];
		constexpr Box_indexer( 
			const Point< double, 6 >		aff_,		//< gdal affine values, [e0, ec, er, n0, nc, nr].
			const Index< rank >				cols_rows_	//< Number of columns and rows.
		) requires( rank == 2 ) :
		    Box_indexer{ 
				BBox(	Pt{	F( aff_[0] ), F( aff_[3] ) }, 
						Pt{	F( aff_[0] + ( col( cols_rows_ ) - 1 )*std::abs( aff_[1] ) ),
							F( aff_[3] + ( row( cols_rows_ ) - 1 )*std::abs( aff_[5] ) ) }
				), 
				Pt{ F( aff_[1] ), F( aff_[5] ) }
			}
		{
			if( ( aff_[2] != 0 ) || ( aff_[4] != 0 ) ) throw std::runtime_error( 
				std::format(	"Oblique affine transformations is not supported, "
								"so aff_[2] and aff_[4] must both be zero in {}.", aff_ ) );
		}

		/// The size of the grid elements.
		constexpr Pt resolution()									const noexcept	{	return m_resolution;	}

		/// Easy access to the superclass.
		constexpr const BBox & box()								const noexcept	{	return *this;			}

		/// Given a point, what offset does it have into the vector of data?
		///	If pt_ is outside the bounding box the result is undefined. So unless you are sure it is not 
		/// outside, you shoud check this with either [Box_indexer::]in_range, strictly_inside, or inside_or_on.
		index_type scalar_index( const Pt & pt_ )					const noexcept	{
			const auto [ ...     pt ]	  = pt_;
			const auto [ ... factor ]	  = m_factor;
			const auto [ ... offset ]	  = m_offset;
			const auto [ ...   exts ]	  = Idx::extents();
			// The exts are necessary to include points with any coordinate value on the max edge:
			return Idx::scalar_index( { smallest( std::fma( pt, factor, offset ), exts - 1 ) ... } );
		}
		
		/// Given an index, returns the coordinates of the element's lower left corner. 
		/// - Mainly used for debugging. 
		/// - Raster_indexer returns the coordinates for the element's upper left corner. 
		///	- If idx_ is >= elements(), the result is undefined. 
		///   So unless you are sure it is ok, you better check it with all_lt( idx_, extents() ). 
		constexpr Pt point( const Index< rank > & idx_ )			const noexcept	{
			const auto [ ...    idx ]	  = idx_;
			const auto [ ... factor ]	  = m_factor;
			const auto [ ... offset ]	  = m_offset;
			return { ( idx - offset )/factor ... };
		}

		/// Return the affine values, in the order specified by gdal.
		/// cons auto aff = box.gdal_affines();
		/// east  = aff[0] + col*aff[1] + row*aff[2];
		/// north = aff[3] + col*aff[4] + row*aff[5];
		constexpr Point< double, 6 > gdal_affines()	const noexcept requires( rank == 2 )	{
			return {	// Negative resolution signifies reversed axis => max instead of min.
				( x( resolution() ) > 0 ) ? x( min() ) : x( max() ),	x( resolution() ),		double{},
				( y( resolution() ) > 0 ) ? y( min() ) : y( max() ),	double{},				y( resolution() )
			};
		}

		/// Box contents as a std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{{{}, {}}}", std::string( box() ), resolution() );
		}
	};
	
	using Box_indexer2d					  = Box_indexer< double, 2 >;

	template< floating F, std::size_t N, arithmetic F2 >
	Box_indexer( const Box< F, N > &, F2 ) -> Box_indexer< F, N >;

	template< floating F, std::size_t N >
	Box_indexer( const Box< F, N > &, Point< F, N > ) -> Box_indexer< F, N >;
	
	template< typename Out, floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Box_indexer< F, N >		  & box_
	) {	return out_ << std::string( box_ );				}

}	// namespace pax
