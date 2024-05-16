#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <vector>
#include <stdexcept>
#include <memory>

#include "primes.hpp"


static void usage(const char* cmdname) {
  std::cerr << "Usage: " << cmdname << " N1 N2 {n_1}" << std::endl;
  std::cerr << "  searches prime constellations where the first member is in [N1, N2)" << std::endl;
  std::cerr << "  prime constellations tuples where" << std::endl;
  std::cerr << "  all members of (p, p+n_1, ..., p+n_k) are prime" << std::endl;
  std::cerr << "Examples for {n_i}" << std::endl;
  std::cerr << "  twins: 2" << std::endl;
  std::cerr << "  cousins: 4" << std::endl;
  std::cerr << "  sexy primes: 6" << std::endl;
  std::cerr << "  triplets: 2 6 or 4 6" << std::endl;
  std::cerr << "  quadruplets: 1 6 8" << std::endl;
  std::cerr << "  sexy quadruplets: 6 12 18" << std::endl;
  std::cerr << "  quintuplets: 2 6 8 12 (OEIS A022006) or" << std::endl;
  std::cerr << "               4 6 10 12 (OEIS A022007) or" << std::endl;
  std::cerr << "  sextuplets: 4 6 10 12 16 (OEIS A022008)" << std::endl;
  std::cerr << "  septuplets: 4 6 8 12 18 20 (OEIS A022009) or" << std::endl;
  std::cerr << "              4 8 12 14 18 20 (OEIS A022010)" << std::endl;
  std::exit(1);
}

static void worker(mpz_class start, mpz_class end, unsigned int k, unsigned int offsets[]) {
  mpz_class prime;
  while (search_prime_constellation(start, end, k, offsets, prime)) {
    std::ostringstream os;
    os << prime << std::endl;
    std::cout << os.str();
    start = prime + 1;
  }
}

int main(int argc, char* argv[]) {
  const char* cmdname = *argv++;
  argc--;
  if (argc < 2) {
    usage(cmdname);
  }

  mpz_class n1, n2;
  try {
    mpz_class val1(*argv++);
    argc--;
    mpz_class val2(*argv++);
    argc--;
    n1 = val1;
    n2 = val2;
  } catch (std::invalid_argument&) {
    usage(cmdname);
  }

  if (n1 < 1 || n2 < 1) {
    std::cerr << cmdname << ": N1 and N2 must be natural numbers" << std::endl;
    usage(cmdname);
  }
  if (n2 <= n1) {
    std::cerr << cmdname << ": N2 must be larger than N1" << std::endl;
    usage(cmdname);
  }

  unsigned k = argc + 1;
  auto offsets = std::make_unique<unsigned int[]>(k - 1);
  for (unsigned int i = 0; i < k - 1; i++) {
    char* endptr;
    auto val = std::strtoul(*argv++, &endptr, 10);
    argc--;
    if (val == 0 || *endptr) {
      std::cerr << cmdname << ": invalid value for n_" << i + 1 << std::endl;
      usage(cmdname);
    } else if (i > 0 && val <= offsets[i - 1]) {
      std::cerr << cmdname << ": n_" << i << " < n_" << i + 1 << " expected" << std::endl;
      usage(cmdname);
    }
    offsets[i] = val;
  }

  int nof_threads = std::thread::hardware_concurrency();
  mpz_class len = n2 - n1 + 1;
  mpz_class min_share;
  mpz_class r;
  mpz_fdiv_qr_ui(min_share.get_mpz_t(), r.get_mpz_t(), len.get_mpz_t(), nof_threads);
  int remainder = mpz_get_si(r.get_mpz_t());

  std::vector<std::thread> threads(nof_threads);
  for (int index = 0; index < nof_threads; index++) {
    mpz_class share = min_share;
    mpz_class start = n1 + min_share * index;
    if (index < remainder) {
      share++;
      start += index;
    } else {
      start += remainder;
    }
    if (start < n2) {
      mpz_class end = start + share;
      threads[index] = std::thread(worker, start, end, k, offsets.get());
    }
  }

  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}