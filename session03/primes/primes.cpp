#include "primes.hpp"

bool search_prime_constellation(
  mpz_class& start_interval, // inclusive
  mpz_class& end_interval,   // exclusive
  unsigned int k,            // size of prime constellation
  unsigned int offsets[],    // k-1 offsets
  mpz_class& first_prime     // found result (if returning true)
) {
  mpz_class p = start_interval - 1; // mpz_nextprime looks for a prime > p
  mpz_class q;
  bool found = false;
  
  for (;;) {
    // lookout for the next prime in the interval
    mpz_nextprime(p.get_mpz_t(), p.get_mpz_t());
    if (p >= end_interval) {
      break;
    }

    // p is apparently prime, check for a constellation
    found = true;
    for (unsigned int i = 0; i < k-1; ++i) {
      unsigned int offset = offsets[i];
      q = p + offset;
      if (mpz_probab_prime_p(q.get_mpz_t(), 50) < 1) {
        found = false;
        break; // not a prime
      }
    }

    if (found) {
      break;
    }
  }

  if (found) {
    first_prime = p;
  }
  return found;
}