//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/table.hpp>

#include <pax/doctest.hpp>
#include <pax/reporting/error_message.hpp>
#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 

#include <sstream>


namespace pax {

	using Size			  = std::size_t;
	static constexpr bool	ignore_chsum_check = true;
	

	/// Calculate the product of the elements.
	template< typename I, Size Rank >
	constexpr auto product( const std::array< I, Rank > & a_ ) {
		return std::accumulate( a_.begin(), a_.end(), I{ 1 }, std::multiplies< I >() );
	}

	/// Calculate the checksum based on the index.
	template< typename I, Size Rank >
	constexpr auto chsum( const std::span< I, Rank > idx_ ) {
		std::remove_cv_t< I >		sum{};
		for( Size i{}; i<Rank; ++i )
			sum = 10*sum + idx_[ Rank - 1 - i ];
		return sum;
	}

	/// Allows looping over non-fixed number of dimensions.
	template< Size Dim = 0, typename I, Size Rank, typename Fn >
		requires( Dim <= Rank )
	constexpr void loop_the_loop(
		const std::span< std::remove_cv_t< I >, Rank >	idx_,
		const std::span< I, Rank >						exts_,
		Fn											 && fn_
	) {
		static constexpr Size	D = Rank - Dim - 1;
		if constexpr( Dim == Rank ) {
			// std::cout << std20::format( "Execute: {}: {} ({})\n", idx_, Dim, exts_ );
			fn_( idx_ );
		} else while( idx_[ D ] < exts_[ D ] ) {
			loop_the_loop< Dim + 1 >( idx_, exts_, fn_ );
			++idx_[ D ];
			for( Size i{}; i < D; ++i )					idx_[ i ] = 0;
		}
	}



	
	/// Create a md_span with extension exts_ and all elements set to zero.
	template< typename Layout, typename T, typename I, Size Rank >
	constexpr auto mdspan0( 
		std::vector< T >			  & data_,
		const std::array< I, Rank >		exts_
	) {
		using MD					  = std::mdspan< T, std::dextents< Size, Rank >, Layout >;
		const Size						prod = product( exts_ );
		if( prod > data_.size() )		data_.resize( prod );
		return MD( data_.data(), exts_ );
	}
	
	/// Create a md_span with extension exts_ and all elements set to chsum.
	template< typename Layout, typename T, typename I, Size Rank >
	constexpr auto mdspan(
		std::vector< T >				  & data_, 
		const std::array< I, Rank >		  & exts_
	) {
		std::array< Size, Rank > 			idx{};
		auto								md = mdspan0< Layout >( data_, exts_ );
		loop_the_loop( 
			std::span( idx ), 
			std::span( exts_ ), 
			[ &md ]( const std::span< Size, Rank > idx_ ){ md[ idx_ ] = chsum( idx_ ); }
		);
		return md;
	}

	/// Check the element values – should be chsum where index is < old_idx_. 
	template< typename T, typename Extents, typename Layout, typename I >
	constexpr void chsum_check( 
		const std::mdspan< T, Extents, Layout >	md_, 
		const std::array< I, Extents::rank() > 	old_idx_,
		const std::source_location				loc_ = std::source_location::current()
	) {
		if constexpr( !ignore_chsum_check ) {
			static constexpr Size		 		Rank = Extents::rank();
		    const auto 							location = to_string( loc_ );
			DOCTEST_CAPTURE( location );

			std::array< Size, Rank >			idx{}, exts{ extents( md_ ) };
			loop_the_loop( 
				std::span( idx ), 
				std::span( exts ), 
				[ &md_, &old_idx_ ]( const std::span< Size, Rank > idx_ ){
					const auto index = std20::format( "{}", idx_ );
					DOCTEST_CAPTURE( index );
						const auto	key = ( idx_ < old_idx_ ) ? chsum( idx_ ) : 0;
						DOCTEST_FAST_CHECK_EQ( md_[ idx_ ], key );
					}
			);
		}
	}

	/// Outputs metadata about the resizeing.
	template< typename T, typename Extents, typename Layout, typename Accessor >
	constexpr void meta_resize(
		const std::mdspan< T, Extents, Layout, Accessor >	srce_, 	///< The source extents.
		const std::mdspan< T, Extents, Layout, Accessor >	dest_ 	///< The destination extents. 
	) {
		if constexpr( !ignore_chsum_check ) {
			static constexpr Size	 	Rank{ Extents::rank() };
			std::array< Size, Rank >	idx{}, smaller_exts{};
			for( Size i{}; i < Rank; ++i )
				smaller_exts[ i ] = std::min( srce_.extent( i ), dest_.extent( i ) );

			loop_the_loop( 
				std::span( idx ), 
				std::span( smaller_exts ), 
				[ &srce_, &dest_ ]( const std::span< Size, Rank > idx_ ){
					std::cout << std20::format( "{}: value {} and {} (index {} and {})\n", idx_,
						srce_[ idx_ ],
						dest_[ idx_ ],
						&srce_[ idx_ ] - srce_.data_handle(),
						&dest_[ idx_ ] - dest_.data_handle()
					);
				}
			);
		}
	}

	/// Outputs metadata about the resizeing.
	template< typename Layout, typename I, Size Rank >
	constexpr void resizer(
		const std::array< I, Rank >		  & srce_,
		const std::array< I, Rank >		  & dest_,
		const std::source_location			loc_ = std::source_location::current()
	) {
		std::vector< int >					data;
		auto								srce = mdspan < Layout >( data, srce_ );
		auto								dest = mdspan0< Layout >( data, dest_ );
		meta_resize( srce, dest );
		resize( srce, dest );
		chsum_check( dest, srce_, loc_ );
	}
	


	DOCTEST_TEST_CASE_TEMPLATE( "mdspan resize", Layout
		// , std::layout_left
		, std::layout_right
	) {
		constexpr std::array		exts1{ 2uz, 2uz, 2uz, 2uz };
		constexpr std::array		exts2{ 4uz, 2uz, 3uz, 5uz };
		{	// Check mdspan( ... )
			{
				std::vector< int >			data;
				constexpr auto				sz{ product( exts1 ) };
				auto						md = mdspan< Layout >( data, exts1 );
				DOCTEST_FAST_CHECK_EQ( data.size(), sz );
				DOCTEST_FAST_CHECK_EQ( md.size(),   sz );
				chsum_check( md, exts1 );
			} {
				std::vector< int >			data;
				constexpr auto				sz{ product( exts2 ) };
				auto						md = mdspan< Layout >( data, exts2 );
				DOCTEST_FAST_CHECK_EQ( data.size(), sz );
				DOCTEST_FAST_CHECK_EQ( md.size(),   sz );
				chsum_check( md, exts2 );
			}

		}
		{	// Check shrinking
			{
				constexpr std::array		dest{ 2uz, 1uz, 2uz, 2uz };
				resizer< Layout >( exts1, dest );
			} {
				constexpr std::array		dest{ 2uz, 1uz, 2uz, 2uz };
				resizer< Layout >( exts2, dest );
			}
		}
	}
	

	
	template< typename Layout, Size Rank = 4 >
	using MDspan = std::mdspan< const int, std::dextents< Size, Rank >, Layout >;

	constexpr auto		 		cols = 8u;
	constexpr auto		 		rows = 4u;
	constexpr std::array		exts{ cols, rows };
	constexpr std::array		arr{
		 0,  1,  2,  3,  4,  5,  6,  7,		// 0
		 8,  9, 10, 11, 12, 13, 14, 15,		// 1
		16, 17, 18, 19, 20, 21, 22, 23,		// 2
		24, 25, 26, 27, 28, 29, 30, 31,		// 3
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
	};
	constexpr std::span			sp0{ arr };
	constexpr std::span			sp { sp0.first( cols*rows ) };


	DOCTEST_TEST_CASE( "Table print" ) { 
		{	// Not csv
			{	// Rank 1
				constexpr std::string_view					result{ "[0, 1, 2, 3, 4]\n" };
				constexpr MDspan< std::layout_right, 1 >	md( sp0.data(), 5 );
				std::stringstream							temp{};
				pax::print( temp, md );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 right
				constexpr std::string_view		result{ "[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 2 >( sp0.data(), 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 left
				constexpr std::string_view		result{ "[0, 1, 2]\n[3, 4, 5]\n[6, 7, 8]\n[9, 10, 11]\n[12, 13, 14]\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_left, 2 >( sp0.data(), 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "\n[0, c, r]:\n[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n\n[1, c, r]:\n[15, 16, 17, 18, 19]\n[20, 21, 22, 23, 24]\n[25, 26, 27, 28, 29]\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 3 >( sp0.data(), 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 right
				constexpr std::string_view		result{ "\n[0, 0, c, r]:\n[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n\n[0, 1, c, r]:\n[15, 16, 17, 18, 19]\n[20, 21, 22, 23, 24]\n[25, 26, 27, 28, 29]\n\n[1, 0, c, r]:\n[30, 31, 32, 33, 34]\n[35, 36, 37, 38, 39]\n[40, 41, 42, 43, 44]\n\n[1, 1, c, r]:\n[45, 46, 47, 48, 49]\n[50, 51, 52, 53, 54]\n[55, 56, 57, 58, 59]\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 4 >( sp0.data(), 2, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 left
				constexpr std::string_view		result{ "\n[r, c, 0, 0]:\n[0, 1]\n[2, 3]\n\n[r, c, 1, 0]:\n[4, 5]\n[6, 7]\n\n[r, c, 2, 0]:\n[8, 9]\n[10, 11]\n\n[r, c, 0, 1]:\n[12, 13]\n[14, 15]\n\n[r, c, 1, 1]:\n[16, 17]\n[18, 19]\n\n[r, c, 2, 1]:\n[20, 21]\n[22, 23]\n\n[r, c, 0, 2]:\n[24, 25]\n[26, 27]\n\n[r, c, 1, 2]:\n[28, 29]\n[30, 31]\n\n[r, c, 2, 2]:\n[32, 33]\n[34, 35]\n\n[r, c, 0, 3]:\n[36, 37]\n[38, 39]\n\n[r, c, 1, 3]:\n[40, 41]\n[42, 43]\n\n[r, c, 2, 3]:\n[44, 45]\n[46, 47]\n\n[r, c, 0, 4]:\n[48, 49]\n[50, 51]\n\n[r, c, 1, 4]:\n[52, 53]\n[54, 55]\n\n[r, c, 2, 4]:\n[56, 57]\n[58, 59]\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_left, 4 >( sp0.data(), 2, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
		}
		{	// csv
			{	// Rank 1
				constexpr std::string_view		result{ "0;1;2;3;4\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 1 >( sp0.data(), 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 right
				constexpr std::string_view		result{ "0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 2 >( sp0.data(), 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 left
				constexpr std::string_view		result{ "0;1;2\n3;4;5\n6;7;8\n9;10;11\n12;13;14\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_left, 2 >( sp0.data(), 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "\n[0, c, r]:\n0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n\n[1, c, r]:\n15;16;17;18;19\n20;21;22;23;24\n25;26;27;28;29\n" };
				std::stringstream				temp{};
				pax::print( temp, MDspan< std::layout_right, 3 >( sp0.data(), 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 right
				constexpr std::string_view		result{ "\n[0, 0, c, r]:\n0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n\n[0, 1, c, r]:\n15;16;17;18;19\n20;21;22;23;24\n25;26;27;28;29\n\n[1, 0, c, r]:\n30;31;32;33;34\n35;36;37;38;39\n40;41;42;43;44\n\n[1, 1, c, r]:\n45;46;47;48;49\n50;51;52;53;54\n55;56;57;58;59\n" };
				constexpr auto					mdsp = MDspan< std::layout_right, 4 >( sp0.data(), 2, 2, 3, 5 );
				std::stringstream				temp{};
				pax::print( temp, mdsp, ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
				pax::print_meta( std::cout, mdsp );
			}
			{	// Rank 4 left
				constexpr std::string_view		result{ "\n[r, c, 0, 0]:\n0;1\n2;3\n\n[r, c, 1, 0]:\n4;5\n6;7\n\n[r, c, 2, 0]:\n8;9\n10;11\n\n[r, c, 0, 1]:\n12;13\n14;15\n\n[r, c, 1, 1]:\n16;17\n18;19\n\n[r, c, 2, 1]:\n20;21\n22;23\n\n[r, c, 0, 2]:\n24;25\n26;27\n\n[r, c, 1, 2]:\n28;29\n30;31\n\n[r, c, 2, 2]:\n32;33\n34;35\n\n[r, c, 0, 3]:\n36;37\n38;39\n\n[r, c, 1, 3]:\n40;41\n42;43\n\n[r, c, 2, 3]:\n44;45\n46;47\n\n[r, c, 0, 4]:\n48;49\n50;51\n\n[r, c, 1, 4]:\n52;53\n54;55\n\n[r, c, 2, 4]:\n56;57\n58;59\n" };
				constexpr auto					mdsp = MDspan< std::layout_left, 4 >( sp0.data(), 2, 2, 3, 5 );
				std::stringstream				temp{};
				pax::print( temp, mdsp, ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
				pax::print_meta( std::cout, mdsp );
			}
		}
	}

	DOCTEST_TEST_CASE( "layout" ) { 
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< Size, 2 >, std::layout_right >;
			const MD			md( sp.data(), exts );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		rows );
			DOCTEST_FAST_CHECK_EQ( md[ 7, 3 ],		3*cols + 7 );
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< Size, 2 >, std::layout_left >;
			const MD			md( sp.data(), exts );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		cols );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 7, 3 ],		3 + rows*7 );
		}
	}
	DOCTEST_TEST_CASE( "begin/end dim 2" ) { 
		constexpr std::array	exts2{ 3, 4 };
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< Size, 2 >, std::layout_right >;
			const MD			md( sp.data(), exts2 );
			{	// Dim 0
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				auto			b = begin< 1 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	4 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< Size, 2 >, std::layout_left >;
			const MD			md( sp.data(), exts2 );
			{	// Dim 0
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	3 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				auto			b = begin< 1 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
		}
	}
	DOCTEST_TEST_CASE( "begin/end dim 3" ) { 
		constexpr std::array	exts3{ 3, 4, 2 };
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< Size, 3 >, std::layout_right >;
			const MD			md( sp.data(), exts3 );
			{	// Dim 0
				auto				b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				// // Items are not sequencial
				// std::vector 	test{	&md[ 0, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 0, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 1 ] - &md[ 0, 1, 0 ] };
				// std::sort( test.begin(), test.end() );
				// std::println( "Items are not sequencial: {}", test );
			}
			{	// Dim 2
				auto				b = begin< 2 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	2 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 2 >( md, 1 ) - b,	0 );
			}
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< Size, 3 >, std::layout_left >;
			const MD			md( sp.data(), exts3 );
			{	// Dim 0
				auto				b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	3 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				// // Items are not sequencial
				// std::vector 	test{	&md[ 0, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 0, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 1 ] - &md[ 0, 1, 0 ] };
				// std::sort( test.begin(), test.end() );
				// std::println( "Items are not sequencial: {}", test );
			}
			{	// Dim 2
				auto				b = begin< 2 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 2 >( md, 1 ) - b,	0 );
			}
		}
	}
	DOCTEST_TEST_CASE( "resize" ) { 
		constexpr std::array	downsize { cols - 2, rows - 1 };
		constexpr std::array	upsize_r { cols,     rows + 2 };
		constexpr std::array	upsize_c { cols + 2, rows     };
		constexpr std::array	upsize_rc{ cols + 2, rows + 2 };

		{	// layout_right
			using MDspan	  = std::mdspan< int, std::dextents< Size, 2 >, std::layout_right >;
			constexpr int		in01{ sp[ 1 ] }, in10{ sp[ rows ] }, in25{ sp[ 2 + rows*5 ] };
			{	// Downsize
				constexpr auto							new_extent = downsize;
				std::vector< int >						data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan									srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],			 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],			in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],			in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],			in25 );

				{
					std::vector< int >					dest_data( data.size() );
					MDspan								dest{ dest_data.data(), new_extent };
					resize( srce, dest );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
					DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
					DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				} {
					MDspan								dest{ data.data(), new_extent };
					resize( srce, dest );
					DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
					DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
					DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				}
			}
			{	// Upsize rows
				constexpr auto						new_extent = upsize_r;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
			}
			{	// Upsize cols
				constexpr auto						new_extent = upsize_c;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
			}
			{	// Upsize rows and cols
				constexpr auto						new_extent = upsize_rc;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				{
					std::vector< int >					dest_data( data.size() );
					MDspan								dest{ dest_data.data(), new_extent };
					resize( srce, dest );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
					DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
					DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ cols, rows ],	 0 );
				} {
					MDspan								dest{ data.data(), new_extent };
					resize( srce, dest );
					DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
					DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
					DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
					DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
					DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
					DOCTEST_FAST_CHECK_EQ( dest[ cols, rows ],	 0 );
				}
			}
		}
		{	// layout_left
			using MDspan	  = std::mdspan< int, std::dextents< Size, 2 >, std::layout_left >;
			constexpr int		in01{ sp[ cols ] }, in10{ sp[ 1 ] }, in25{ sp[ cols*2 + 5 ] };
			{	// Downsize
				constexpr auto						new_extent = downsize;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
			}
			{	// Upsize rows
				constexpr auto						new_extent = upsize_r;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
			}
			{	// Upsize cols
				constexpr auto						new_extent = upsize_c;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
			}
			{	// Upsize rows and cols
				constexpr auto						new_extent = upsize_rc;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), exts };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, rows ],	 0 );
			}
		}
	}

}	// namespace pax
