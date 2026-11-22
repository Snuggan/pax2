# A Polynomial template

A template for polynomials accompanied by various support functions. All templates are ``constexpr``.

The actual template, ``pax::Polynomial< typename Algo, floating_point F, std::size_t N >``, is a rather simple template where ``F`` is the value type and ``N`` is the static size (unless ``N`` is ``std::dynamic_extent``). The base type is ``std::array< F, N >`` for static sizes and ``std::vector< F >`` otherwise. The first template argument, ``Algo``, is the algorithm to use to calculate the value of the polynomial at a specific point. You can supply your own, if need be. The template has only four member functions: ``operator()``, ``operator==``, ``operator<=>``, and ``degree()`` which returns the polynomial degree. ``operator()`` calculates the value of the polynomial in a given point using ``Algo``. 

One algorithm is included: ``pax::horner_fma`` implements the standard Horner algorithm using ``std::fma``. Two variants, ``pax::horner_fma_even`` and ``pax::horner_fma_odd``, may to be used with polynomials where every second coefficient is zero and can be ignored – and they are ignored, no check that they actually are zero is done. 

## Support functions

There are three functions ``polynomial`` to use when constructing polynomials. One takes any number of scalar values, the coefficients of the polynomial. One takes a ``std::span``. One takes a c-style array. 

There are arithmetic operators for addition, subtraction, multiplication, and division of a polynomial and another polynomial or scalar. 

There are also three additional functions: ``derivate``, ``integrate``, and ``shift< n >``. The latter is the same as multiplying the polynomial with ``x^n``, but is more efficient. 

## Named polynomials

The accompanied file ``polynomial-named.hpp`` implements a number of polynomials, they are all ``constexpr`` and calculated during compilation.

- ``pax::poly::all_one< N >``, where all coefficients are one. 
- ``pax::poly::high_one< N >``, is the same as ``x^N``.
- Abel polynomials: ``pax::poly::abel< N >( a )``, [Wikipedia](https://en.wikipedia.org/wiki/Abel_polynomials).
- Bessel polynomials: ``pax::poly::bessel< N >`` and ``pax::poly::bessel_reversed< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Bessel_polynomials).
- Chebyshev polynomials of the first and second kind: ``pax::poly::chebyshev1< N >`` and ``pax::poly::chebyshev2< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Chebyshev_polynomials).
- Dickson polynomials of the first and second kind: ``pax::poly::dickson1< N >( a )`` and ``pax::poly::dickson2< N >( a )``, [Wikipedia](https://en.wikipedia.org/wiki/Dickson_polynomial).
- Fibonacci polynomials: ``pax::poly::fibonacci< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Fibonacci_polynomials).
- Hermite polynomials, probabilist's and physicist's: ``pax::poly::hermite1< N >`` and ``pax::poly::hermite2< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Hermite_polynomials).
- Jacobi polynomials: ``pax::poly::jacobi< N >( alpha, beta )``, [Wikipedia](https://en.wikipedia.org/wiki/Jacobi_polynomials).
- Laguerre and generalized laguerre polynomials: ``pax::poly::laguerre< N >`` and ``pax::poly::laguerre_generalized< N >( a )``, [Wikipedia](https://en.wikipedia.org/wiki/Laguerre_polynomials).
- Legendre polynomials: ``pax::poly::legendre< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Legendre_polynomials).
- Lucas polynomials: ``pax::poly::lucas< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Fibonacci_polynomials).
- Shapiro polynomials of the first and second kind: ``pax::poly::shapiro1< N >`` and ``pax::poly::shapiro2< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Shapiro_polynomials).
- Touchard polynomials: ``pax::poly::touchard< N >``, [Wikipedia](https://en.wikipedia.org/wiki/Touchard_polynomials).
- Wilkinson's polynomial: ``pax::poly::wilkinsons``, [Wikipedia](https://en.wikipedia.org/wiki/Wilkinson%27s_polynomial).
