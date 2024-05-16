#ifndef PRIMES_HPP
#define PRIMES_HPP

#include <gmp.h>
#include <gmpxx.h>

bool search_prime_constellation(
  mpz_class& start_interval, // inclusive
  mpz_class& end_interval,   // exclusive
  unsigned int k,            // size of prime constellation
  unsigned int offsets[],    // k-1 offsets
  mpz_class& first_prime);   // found result (if returning true)
#endif