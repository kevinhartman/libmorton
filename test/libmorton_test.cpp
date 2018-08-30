// Libmorton Tests
// This is a program designed to test and benchmark the functionality offered by the libmorton library
//
// Jeroen Baert 2015

// Utility headers
#include "libmorton_test.h"
#include "libmorton_test_2D.h"
#include "libmorton_test_3D.h"

using namespace std;
using namespace std::chrono;
using namespace libmorton;

/// GLOBALS
// Configuration
size_t MAX;
unsigned int times;
size_t total;
size_t RAND_POOL_SIZE = 10000;
// Runningsums
vector<uint_fast64_t> running_sums;
/// END GLOBALS

// Morton ND LUT (N = 2)
constexpr auto MortonND_2D_32_4 = mortonnd::MortonNDLutEncoder<2, 16, 4>(); // 4 chunks per field, 4 bits each = 16 per field
constexpr uint_fast32_t MortonND_2D_32_4_Encode(const uint_fast16_t x, const uint_fast16_t y) {
	return MortonND_2D_32_4.Encode(x, y);
}

constexpr auto MortonND_2D_32_8 = mortonnd::MortonNDLutEncoder<2, 16, 8>(); // 2 chunks per field, 8 bits each = 16 per field
constexpr uint_fast32_t MortonND_2D_32_8_Encode(const uint_fast16_t x, const uint_fast16_t y) {
	return MortonND_2D_32_8.Encode(x, y);
}

constexpr auto MortonND_2D_64_8 = mortonnd::MortonNDLutEncoder<2, 32, 8>(); // 4 chunks per field, 8 bits each = 32 per field
constexpr uint_fast64_t MortonND_2D_64_8_Encode(const uint_fast32_t x, const uint_fast32_t y) {
	return MortonND_2D_64_8.Encode(x, y);
}

// Morton ND LUT (N = 3)
constexpr auto MortonND_3D_32_5 = mortonnd::MortonNDLutEncoder<3, 10, 5>(); // 2 chunks per field, 5 bits each = 10 per field
constexpr uint_fast32_t MortonND_3D_32_5_Encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	return MortonND_3D_32_5.Encode(x, y, z);
}

constexpr auto MortonND_3D_32_8 = mortonnd::MortonNDLutEncoder<3, 10, 8>(); //
constexpr uint_fast32_t MortonND_3D_32_8_Encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	return MortonND_3D_32_8.Encode(x, y, z);
}

constexpr auto MortonND_3D_32_10 = mortonnd::MortonNDLutEncoder<3, 10, 10>(); // 1 chunks per field, 10 bits each = 10 per field
constexpr uint_fast32_t MortonND_3D_32_10_Encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	return MortonND_3D_32_10.Encode(x, y, z);
}

constexpr auto MortonND_3D_64_7 = mortonnd::MortonNDLutEncoder<3, 21, 7>(); // 3 chunks per field, 7 bits each = 21 per field
constexpr uint_fast64_t MortonND_3D_64_7_Encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z) {
	return MortonND_3D_64_7.Encode(x, y, z);
}

// 3D functions collections
vector<encode_3D_64_wrapper> f3D_64_encode; // 3D 64-bit encode functions
vector<encode_3D_32_wrapper> f3D_32_encode; // 3D 32_bit encode functions
vector<decode_3D_64_wrapper> f3D_64_decode; // 3D 64-bit decode functions
vector<decode_3D_32_wrapper> f3D_32_decode; // 3D 32_bit decode functions
// 2D functions collections
vector<encode_2D_64_wrapper> f2D_64_encode; // 2D 64-bit encode functions
vector<encode_2D_32_wrapper> f2D_32_encode; // 2D 32_bit encode functions
vector<decode_2D_64_wrapper> f2D_64_decode; // 2D 64-bit decode functions
vector<decode_2D_32_wrapper> f2D_32_decode; // 2D 32_bit decode functions

// Make a total of all running_sum checks and print it
// This is an elaborate way to ensure no function call gets optimized away
void printRunningSums(){
	uint_fast64_t t = 0;
	cout << "Running sums check: ";
	for(int i = 0; i < running_sums.size(); i++) {
		t+= running_sums[i];
	}
	cout << t << endl;
}

template <typename morton, typename coord>
static std::string testEncode_3D_Perf(morton(*function)(coord, coord, coord), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testEncode_3D_Linear_Perf<morton, coord>(function, times) << " ms " 
		<< testEncode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

template <typename morton, typename coord>
static std::string testDecode_3D_Perf(void(*function)(const morton, coord&, coord&, coord&), size_t times) {
	stringstream os;
	os << setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << testDecode_3D_Linear_Perf<morton, coord>(function, times) << " ms "
		<< testDecode_3D_Random_Perf<morton, coord>(function, times) << " ms";
	return os.str();
}

static void Encode_3D_Perf() {
	cout << "++ Encoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (auto it = f3D_64_encode.begin(); it != f3D_64_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (auto it = f3D_32_encode.begin(); it != f3D_32_encode.end(); it++) {
		cout << "    " << testEncode_3D_Perf((*it).encode, times) << " : 32-bit " << (*it).description << endl;
	}
}

inline static void Decode_3D_Perf() {
	cout << "++ Decoding " << MAX << "^3 morton codes (" << total << " in total)" << endl;
	for (auto it = f3D_64_decode.begin(); it != f3D_64_decode.end(); it++) {
		cout << "    " << testDecode_3D_Perf((*it).decode, times) << " : 64-bit " << (*it).description << endl;
	}
	for (auto it = f3D_32_decode.begin(); it != f3D_32_decode.end(); it++) {
		cout << "    " << testDecode_3D_Perf((*it).decode, times) << " : 32-bit " << (*it).description << endl;
	}
}

void printHeader(){
	cout << "LIBMORTON TEST SUITE" << endl;
	cout << "--------------------" << endl;
#if _WIN64 || __x86_64__  
	cout << "++ 64-bit version" << endl;
#else
	cout << "++ 32-bit version" << endl;
#endif
#if _MSC_VER
	cout << "++ Compiled using MSVC" << endl;
#elif __GNUC__
	cout << "++ Compiled using GCC" << endl;
#endif
}

#if _MSC_VER
void registerIfNotMSVC() {
	// MSVC is not compatible with Morton ND LUT sizes > 10
}
#else
constexpr auto MortonND_2D_32_16 = mortonnd::MortonNDLutEncoder<2, 16, 16>(); // 1 chunks per field, 16 bits each = 16 per field
constexpr uint_fast32_t MortonND_2D_32_16_Encode(const uint_fast16_t x, const uint_fast16_t y) {
	return MortonND_2D_32_16.Encode(x, y);
}

constexpr auto MortonND_3D_64_16 = mortonnd::MortonNDLutEncoder<3, 21, 16>();
constexpr uint_fast64_t MortonND_3D_64_16_Encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z) {
	return MortonND_3D_64_16.Encode(x, y, z);
}

constexpr auto MortonND_3D_64_21 = mortonnd::MortonNDLutEncoder<3, 21, 21>(); // 3 chunks per field, 7 bits each = 21 per field
constexpr uint_fast64_t MortonND_3D_64_21_Encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z) {
	return MortonND_3D_64_21.Encode(x, y, z);
}

constexpr auto MortonND_2D_64_16 = mortonnd::MortonNDLutEncoder<2, 32, 16>(); // 2 chunks per field, 16 bits each = 32 per field
constexpr uint_fast64_t MortonND_2D_64_16_Encode(const uint_fast32_t x, const uint_fast32_t y) {
	return MortonND_2D_64_16.Encode(x, y);
}

void registerIfNotMSVC() {
	f2D_32_encode.push_back(encode_2D_32_wrapper("MortonND: 1 chunks, 16 bit LUT", &MortonND_2D_32_16_Encode));
	f2D_64_encode.push_back(encode_2D_64_wrapper("MortonND: 2 chunks, 16 bit LUT", &MortonND_2D_64_16_Encode));
	f3D_64_encode.push_back(encode_3D_64_wrapper("MortonND: 2 chunks, 16 bit LUT", &MortonND_3D_64_16_Encode));
	f3D_64_encode.push_back(encode_3D_64_wrapper("MortonND: 1 chunks, 21 bit LUT", &MortonND_3D_64_21_Encode));
}
#endif

#if defined(__BMI2__) || __AVX2__
// Register 3D BMI intrinsics if available

// Morton ND BMI2 (N = 2)
using MortonND_2D_32_BMI = mortonnd::MortonNDBmi<2, uint32_t>;
static inline uint_fast32_t MortonND_2D_32_BMI_Encode(const uint_fast16_t x, const uint_fast16_t y) {
	return MortonND_2D_32_BMI::Encode(x, y);
}

static inline void MortonND_2D_32_BMI_Decode(const uint_fast32_t encoding, uint_fast16_t& x, uint_fast16_t& y) {
	std::tie(x, y) = MortonND_2D_32_BMI::Decode(encoding);
}

using MortonND_2D_64_BMI = mortonnd::MortonNDBmi<2, uint64_t>;
static inline uint_fast64_t MortonND_2D_64_BMI_Encode(const uint_fast32_t x, const uint_fast32_t y) {
	return MortonND_2D_64_BMI::Encode(x, y);
}

static inline void MortonND_2D_64_BMI_Decode(const uint_fast64_t encoding, uint_fast32_t& x, uint_fast32_t& y) {
	std::tie(x, y) = MortonND_2D_64_BMI::Decode(encoding);
}

// Morton ND BMI2 (N = 3)
using MortonND_3D_32_BMI = mortonnd::MortonNDBmi<3, uint32_t>;
static inline uint_fast32_t MortonND_3D_32_BMI_Encode(const uint_fast16_t x, const uint_fast16_t y, const uint_fast16_t z) {
	return MortonND_3D_32_BMI::Encode(x, y, z);
}

static inline void MortonND_3D_32_BMI_Decode(const uint_fast32_t encoding, uint_fast16_t& x, uint_fast16_t& y, uint_fast16_t& z) {
	std::tie(x, y, z) = MortonND_3D_32_BMI::Decode(encoding);
}

using MortonND_3D_64_BMI = mortonnd::MortonNDBmi<3, uint64_t>;
static inline uint_fast64_t MortonND_3D_64_BMI_Encode(const uint_fast32_t x, const uint_fast32_t y, const uint_fast32_t z) {
	return MortonND_3D_64_BMI::Encode(x, y, z);
}

static inline void MortonND_3D_64_BMI_Decode(const uint_fast64_t encoding, uint_fast32_t& x, uint_fast32_t& y, uint_fast32_t& z) {
	std::tie(x, y, z) = MortonND_3D_64_BMI::Decode(encoding);
}

void registerBMI2Functions() {
	f3D_64_encode.push_back(encode_3D_64_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("BMI2 instruction set", &m3D_e_BMI<uint_fast32_t, uint_fast16_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast64_t, uint_fast32_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("BMI2 Instruction set", &m3D_d_BMI<uint_fast32_t, uint_fast16_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("MortonND: BMI2", &MortonND_3D_64_BMI_Encode));
	f3D_32_encode.push_back(encode_3D_32_wrapper("MortonND: BMI2", &MortonND_3D_32_BMI_Encode));
	f2D_64_encode.push_back(encode_2D_64_wrapper("MortonND: BMI2", &MortonND_2D_64_BMI_Encode));
	f2D_32_encode.push_back(encode_2D_32_wrapper("MortonND: BMI2", &MortonND_2D_32_BMI_Encode));

	f3D_64_decode.push_back(decode_3D_64_wrapper("MortonND: BMI2", &MortonND_3D_64_BMI_Decode));
	f3D_32_decode.push_back(decode_3D_32_wrapper("MortonND: BMI2", &MortonND_3D_32_BMI_Decode));
	f2D_64_decode.push_back(decode_2D_64_wrapper("MortonND: BMI2", &MortonND_2D_64_BMI_Decode));
	f2D_32_decode.push_back(decode_2D_32_wrapper("MortonND: BMI2", &MortonND_2D_32_BMI_Decode));
}
#else
void registerBMI2Functions() { }
#endif

// Register all the functions we want to be tested here!
void registerFunctions() {
	// Register 3D 64-bit encode functions
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Shifted ET", &m3D_e_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT Shifted", &m3D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT ET", &m3D_e_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("LUT", &m3D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("Magicbits", &m3D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For ET", &m3D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_encode.push_back(encode_3D_64_wrapper("For", &m3D_e_for<uint_fast64_t, uint_fast32_t>));

	f3D_64_encode.push_back(encode_3D_64_wrapper("MortonND: 3 chunks, 7 bit LUT", &MortonND_3D_64_7_Encode));
	
	// Register 3D 32-bit encode functions
	f3D_32_encode.push_back(encode_3D_32_wrapper("For", &m3D_e_for<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("For ET", &m3D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("Magicbits", &m3D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT", &m3D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT ET", [](uint_fast16_t x, uint_fast16_t y, uint_fast16_t z) -> uint_fast32_t { return m3D_e_LUT_ET<uint_fast32_t, uint_fast32_t>(x, y, z); }));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Shifted", &m3D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_encode.push_back(encode_3D_32_wrapper("LUT Shifted ET", [](uint_fast16_t x, uint_fast16_t y, uint_fast16_t z) -> uint_fast32_t { return m3D_e_sLUT_ET<uint_fast32_t, uint_fast32_t>(x, y, z); }));

	f3D_32_encode.push_back(encode_3D_32_wrapper("MortonND: 2 chunks, 5 bit LUT", &MortonND_3D_32_5_Encode));
	f3D_32_encode.push_back(encode_3D_32_wrapper("MortonND: 2 chunks, 8 bit LUT", &MortonND_3D_32_8_Encode));
	f3D_32_encode.push_back(encode_3D_32_wrapper("MortonND: 1 chunks, 10 bit LUT", &MortonND_3D_32_10_Encode));

	// Register 3D 64-bit decode functions
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Shifted ET", &m3D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT Shifted", &m3D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT ET", &m3D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("LUT", &m3D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("Magicbits", &m3D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For ET", &m3D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f3D_64_decode.push_back(decode_3D_64_wrapper("For", &m3D_d_for<uint_fast64_t, uint_fast32_t>));

	// Register 3D 32-bit decode functions
	f3D_32_decode.push_back(decode_3D_32_wrapper("For", &m3D_d_for<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("For ET", &m3D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("Magicbits", &m3D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT", &m3D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT ET", &m3D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Shifted", &m3D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f3D_32_decode.push_back(decode_3D_32_wrapper("LUT Shifted ET", &m3D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	// Register 2D 64-bit encode functions
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Shifted ET", [](uint_fast32_t x, uint_fast32_t y) -> uint_fast64_t { return m2D_e_sLUT_ET<uint_fast64_t, uint_fast16_t>((uint_fast16_t)x, (uint_fast16_t)y); }));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT Shifted", &m2D_e_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT ET", [](uint_fast32_t x, uint_fast32_t y) -> uint_fast64_t { return m2D_e_LUT_ET<uint_fast64_t, uint_fast16_t>((uint_fast16_t)x, (uint_fast16_t)y); }));
	f2D_64_encode.push_back(encode_2D_64_wrapper("LUT", &m2D_e_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("Magicbits", &m2D_e_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For ET", &m2D_e_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("For", &m2D_e_for<uint_fast64_t, uint_fast32_t>));
	f2D_64_encode.push_back(encode_2D_64_wrapper("MortonND: 4 chunks, 8 bit LUT", MortonND_2D_64_8_Encode));

	// Register 2D 32-bit encode functions
	f2D_32_encode.push_back(encode_2D_32_wrapper("For", &m2D_e_for<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("For ET", &m2D_e_for_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("Magicbits", &m2D_e_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT", &m2D_e_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT ET", &m2D_e_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Shifted", &m2D_e_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_encode.push_back(encode_2D_32_wrapper("LUT Shifted ET", &m2D_e_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	f2D_32_encode.push_back(encode_2D_32_wrapper("MortonND: 4 chunks, 4 bit LUT", &MortonND_2D_32_4_Encode));
	f2D_32_encode.push_back(encode_2D_32_wrapper("MortonND: 2 chunks, 8 bit LUT", &MortonND_2D_32_8_Encode));

	// Register 2D 64-bit decode functions
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Shifted ET", &m2D_d_sLUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT Shifted", &m2D_d_sLUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT ET", &m2D_d_LUT_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("LUT", &m2D_d_LUT<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("Magicbits", &m2D_d_magicbits<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For ET", &m2D_d_for_ET<uint_fast64_t, uint_fast32_t>));
	f2D_64_decode.push_back(decode_2D_64_wrapper("For", &m2D_d_for<uint_fast64_t, uint_fast32_t>));

	// Register 2D 32-bit decode functions
	f2D_32_decode.push_back(decode_2D_32_wrapper("For", &m2D_d_for<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("For ET", &m2D_d_for_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("Magicbits", &m2D_d_magicbits<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT", &m2D_d_LUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT ET", &m2D_d_LUT_ET<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Shifted", &m2D_d_sLUT<uint_fast32_t, uint_fast16_t>));
	f2D_32_decode.push_back(decode_2D_32_wrapper("LUT Shifted ET", &m2D_d_sLUT_ET<uint_fast32_t, uint_fast16_t>));

	registerBMI2Functions();
	registerIfNotMSVC();
}

int main(int argc, char *argv[]) {
	times = 1;
	printHeader();

	// register functions
	registerFunctions();

	cout << "++ Checking 3D methods for correctness" << endl;
	check3D_EncodeDecodeMatch<uint_fast64_t, uint_fast32_t>(f3D_64_encode, f3D_64_decode, times);
	check3D_EncodeDecodeMatch<uint_fast32_t, uint_fast16_t>(f3D_32_encode, f3D_32_decode, times);
	check3D_EncodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f3D_64_encode);
	check3D_EncodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f3D_32_encode);
	check3D_DecodeCorrectness<uint_fast64_t, uint_fast32_t>(f3D_64_decode);
	check3D_DecodeCorrectness<uint_fast32_t, uint_fast16_t>(f3D_32_decode);

	cout << "++ Checking 2D methods for correctness" << endl;
	check2D_EncodeCorrectness<uint_fast64_t, uint_fast32_t, 64>(f2D_64_encode);
	check2D_EncodeCorrectness<uint_fast32_t, uint_fast16_t, 32>(f2D_32_encode);
	// TODO: check 2D decode for correctness
	
	cout << "++ Running each performance test " << times << " times and averaging results" << endl;
	for (int i = 128; i <= 512; i = i * 2){
		MAX = i;
		total = MAX*MAX*MAX;
		Encode_3D_Perf();
		Decode_3D_Perf();
		printRunningSums();
	}
}
