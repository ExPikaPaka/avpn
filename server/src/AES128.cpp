#include "AES128.h"

AES128::AES128() {};

string AES128::encrypt(const string& massage, const string& keytext) {
    string plaintext = stringToHex(massage);
    auto key = std::unique_ptr<unsigned char[]>(string2hex(keytext, 32));
    auto expanded_key = std::unique_ptr<unsigned char[]>(ExpandKey(key.get()));

    int n = plaintext.length();
    string total_enc = "";

    for (int part = 0; part < (n + 31) / 32; part++) {
        int cutoff = min(n, (part + 1) * 32);
        string part_string = plaintext.substr(part * 32, cutoff);
        for (int i = 0; cutoff % 32 != 0 && i < 32 - cutoff % 32; i++) {
            part_string += "0";
        }
        auto padded_string = std::unique_ptr<unsigned char[]>(string2hex(part_string, 32));
        auto cipher = std::unique_ptr<unsigned char[]>(Encrypt(padded_string.get(), expanded_key.get()));
        string res = hex2string(cipher.get(), 16);
        total_enc += res;
    }
    return total_enc;
}

string AES128::decrypt(const string& total_enc, const string& keytext) {
    string total_dec = "";
    auto key = std::unique_ptr<unsigned char[]>(string2hex(keytext, 32));
    auto expanded_key = std::unique_ptr<unsigned char[]>(ExpandKey(key.get()));
    int n = total_enc.length();

    for (int part = 0; part < (total_enc.length() + 31) / 32; part++) {
        int cutoff = min(n, (part + 1) * 32);
        string part_string = total_enc.substr(part * 32, cutoff);
        for (int i = 0; cutoff % 32 != 0 && i < 32 - cutoff % 32; i++) {
            part_string += "0";
        }
        auto padded_string = std::unique_ptr<unsigned char[]>(string2hex(part_string, 32));
        auto reverse_cipher = std::unique_ptr<unsigned char[]>(Decrypt(padded_string.get(), expanded_key.get()));
        string dec = hex2string(reverse_cipher.get(), 16);
        total_dec += dec;
    }
    return hexToString(total_dec);
}

unsigned char AES128::gmul(unsigned char a, unsigned char b) {
	unsigned char p = 0;
	while (a && b) {
		if (b & 1)
			p ^= a;

		if (a & 0x80)
			a = (a << 1) ^ 0x11b;
		else
			a <<= 1;
		b >>= 1;
	}
	return p;
}

unsigned char AES128::frcon(unsigned char i)
{
	if (i == 0)
		return 0x8d;
	unsigned char res = 1;
	for (unsigned char x = 1; x < i; x++)
	{
		res = gmul(res, 2);
	}
	return res;
}

void AES128::SubWordRotWordXOR(unsigned char* temp_word, unsigned char i) {
	unsigned char temp = temp_word[0];
	temp_word[0] = temp_word[1];
	temp_word[1] = temp_word[2];
	temp_word[2] = temp_word[3];
	temp_word[3] = temp;

	temp_word[0] = s[temp_word[0]];
	temp_word[1] = s[temp_word[1]];
	temp_word[2] = s[temp_word[2]];
	temp_word[3] = s[temp_word[3]];

	temp_word[0] ^= frcon(i);
	// other 3 bytes are XORed with 0
}

unsigned char* AES128::ExpandKey(unsigned char key[16]) {
	unsigned char* expanded_key = new unsigned char[176];

	for (int i = 0; i < 16; i++) {
		expanded_key[i] = key[i];
	}

	int bytes_count = 16;
	int rcon_i = 1;
	unsigned char temp[4];

	while (bytes_count < 176) {
		for (int i = 0; i < 4; i++) {
			temp[i] = expanded_key[i + bytes_count - 4];
		}

		if (bytes_count % 16 == 0) {
			SubWordRotWordXOR(temp, rcon_i++);
		}

		for (unsigned char a = 0; a < 4; a++) {
			expanded_key[bytes_count] = expanded_key[bytes_count - 16] ^ temp[a];
			bytes_count++;
		}
	}

	return expanded_key;
}

void AES128::AddSubRoundKey(unsigned char* state, unsigned char* round_key) {
	for (int i = 0; i < 16; i++) {
		state[i] ^= round_key[i];
	}
}

void AES128::EncSubBytes(unsigned char* state) {
	for (int i = 0; i < 16; i++) {
		state[i] = s[state[i]];
	}
}

void AES128::LeftShiftRows(unsigned char* state) {
	unsigned char temp_state[16];

	temp_state[0] = state[0];
	temp_state[1] = state[5];
	temp_state[2] = state[10];
	temp_state[3] = state[15];

	temp_state[4] = state[4];
	temp_state[5] = state[9];
	temp_state[6] = state[14];
	temp_state[7] = state[3];

	temp_state[8] = state[8];
	temp_state[9] = state[13];
	temp_state[10] = state[2];
	temp_state[11] = state[7];

	temp_state[12] = state[12];
	temp_state[13] = state[1];
	temp_state[14] = state[6];
	temp_state[15] = state[11];

	for (int i = 0; i < 16; i++) {
		state[i] = temp_state[i];
	}
}

void AES128::MixColumns(unsigned char* state) {
	unsigned char temp_state[16];

	temp_state[0] = (unsigned char)(gmul(state[0], 2) ^ gmul(state[1], 3) ^ state[2] ^ state[3]);
	temp_state[1] = (unsigned char)(state[0] ^ gmul(state[1], 2) ^ gmul(state[2], 3) ^ state[3]);
	temp_state[2] = (unsigned char)(state[0] ^ state[1] ^ gmul(state[2], 2) ^ gmul(state[3], 3));
	temp_state[3] = (unsigned char)(gmul(state[0], 3) ^ state[1] ^ state[2] ^ gmul(state[3], 2));

	temp_state[4] = (unsigned char)(gmul(state[4], 2) ^ gmul(state[5], 3) ^ state[6] ^ state[7]);
	temp_state[5] = (unsigned char)(state[4] ^ gmul(state[5], 2) ^ gmul(state[6], 3) ^ state[7]);
	temp_state[6] = (unsigned char)(state[4] ^ state[5] ^ gmul(state[6], 2) ^ gmul(state[7], 3));
	temp_state[7] = (unsigned char)(gmul(state[4], 3) ^ state[5] ^ state[6] ^ gmul(state[7], 2));

	temp_state[8] = (unsigned char)(gmul(state[8], 2) ^ gmul(state[9], 3) ^ state[10] ^ state[11]);
	temp_state[9] = (unsigned char)(state[8] ^ gmul(state[9], 2) ^ gmul(state[10], 3) ^ state[11]);
	temp_state[10] = (unsigned char)(state[8] ^ state[9] ^ gmul(state[10], 2) ^ gmul(state[11], 3));
	temp_state[11] = (unsigned char)(gmul(state[8], 3) ^ state[9] ^ state[10] ^ gmul(state[11], 2));

	temp_state[12] = (unsigned char)(gmul(state[12], 2) ^ gmul(state[13], 3) ^ state[14] ^ state[15]);
	temp_state[13] = (unsigned char)(state[12] ^ gmul(state[13], 2) ^ gmul(state[14], 3) ^ state[15]);
	temp_state[14] = (unsigned char)(state[12] ^ state[13] ^ gmul(state[14], 2) ^ gmul(state[15], 3));
	temp_state[15] = (unsigned char)(gmul(state[12], 3) ^ state[13] ^ state[14] ^ gmul(state[15], 2));

	for (int i = 0; i < 16; i++) {
		state[i] = temp_state[i];
	}
}

unsigned char* AES128::Encrypt(unsigned char* plaintext, unsigned char* expanded_key) {
	unsigned char state[16];
	unsigned char* cipher = new unsigned char[16];

	for (int i = 0; i < 16; i++) {
		state[i] = plaintext[i];
	}

	AddSubRoundKey(state, expanded_key);

	for (int i = 1; i <= 9; i++) {
		EncSubBytes(state);
		LeftShiftRows(state);
		MixColumns(state);
		AddSubRoundKey(state, expanded_key + (16 * i));
	}

	EncSubBytes(state);
	LeftShiftRows(state);
	AddSubRoundKey(state, expanded_key + 160);

	for (int i = 0; i < 16; i++) {
		cipher[i] = state[i];
	}

	return cipher;
}

void AES128::InverseMixColumns(unsigned char* state) {
	unsigned char temp_state[16];

	temp_state[0] = (unsigned char)(gmul(state[0], 14) ^ gmul(state[1], 11) ^ gmul(state[2], 13) ^ gmul(state[3], 9));
	temp_state[1] = (unsigned char)(gmul(state[0], 9) ^ gmul(state[1], 14) ^ gmul(state[2], 11) ^ gmul(state[3], 13));
	temp_state[2] = (unsigned char)(gmul(state[0], 13) ^ gmul(state[1], 9) ^ gmul(state[2], 14) ^ gmul(state[3], 11));
	temp_state[3] = (unsigned char)(gmul(state[0], 11) ^ gmul(state[1], 13) ^ gmul(state[2], 9) ^ gmul(state[3], 14));

	temp_state[4] = (unsigned char)(gmul(state[4], 14) ^ gmul(state[5], 11) ^ gmul(state[6], 13) ^ gmul(state[7], 9));
	temp_state[5] = (unsigned char)(gmul(state[4], 9) ^ gmul(state[5], 14) ^ gmul(state[6], 11) ^ gmul(state[7], 13));
	temp_state[6] = (unsigned char)(gmul(state[4], 13) ^ gmul(state[5], 9) ^ gmul(state[6], 14) ^ gmul(state[7], 11));
	temp_state[7] = (unsigned char)(gmul(state[4], 11) ^ gmul(state[5], 13) ^ gmul(state[6], 9) ^ gmul(state[7], 14));

	temp_state[8] = (unsigned char)(gmul(state[8], 14) ^ gmul(state[9], 11) ^ gmul(state[10], 13) ^ gmul(state[11], 9));
	temp_state[9] = (unsigned char)(gmul(state[8], 9) ^ gmul(state[9], 14) ^ gmul(state[10], 11) ^ gmul(state[11], 13));
	temp_state[10] = (unsigned char)(gmul(state[8], 13) ^ gmul(state[9], 9) ^ gmul(state[10], 14) ^ gmul(state[11], 11));
	temp_state[11] = (unsigned char)(gmul(state[8], 11) ^ gmul(state[9], 13) ^ gmul(state[10], 9) ^ gmul(state[11], 14));

	temp_state[12] = (unsigned char)(gmul(state[12], 14) ^ gmul(state[13], 11) ^ gmul(state[14], 13) ^ gmul(state[15], 9));
	temp_state[13] = (unsigned char)(gmul(state[12], 9) ^ gmul(state[13], 14) ^ gmul(state[14], 11) ^ gmul(state[15], 13));
	temp_state[14] = (unsigned char)(gmul(state[12], 13) ^ gmul(state[13], 9) ^ gmul(state[14], 14) ^ gmul(state[15], 11));
	temp_state[15] = (unsigned char)(gmul(state[12], 11) ^ gmul(state[13], 13) ^ gmul(state[14], 9) ^ gmul(state[15], 14));

	for (int i = 0; i < 16; i++) {
		state[i] = temp_state[i];
	}
}

void AES128::RightShiftRows(unsigned char* state) {
	unsigned char temp_state[16];

	temp_state[0] = state[0];
	temp_state[1] = state[13];
	temp_state[2] = state[10];
	temp_state[3] = state[7];

	temp_state[4] = state[4];
	temp_state[5] = state[1];
	temp_state[6] = state[14];
	temp_state[7] = state[11];

	temp_state[8] = state[8];
	temp_state[9] = state[5];
	temp_state[10] = state[2];
	temp_state[11] = state[15];

	temp_state[12] = state[12];
	temp_state[13] = state[9];
	temp_state[14] = state[6];
	temp_state[15] = state[3];

	for (int i = 0; i < 16; i++) {
		state[i] = temp_state[i];
	}
}

void AES128::DecSubBytes(unsigned char* state) {
	for (int i = 0; i < 16; i++) {
		state[i] = inv_s[state[i]];
	}
}

unsigned char* AES128::Decrypt(unsigned char* cipher, unsigned char* expanded_key)
{
	unsigned char state[16];
	unsigned char* plaintext = new unsigned char[16];

	for (int i = 0; i < 16; i++) {
		state[i] = cipher[i];
	}

	AddSubRoundKey(state, expanded_key + 160);
	RightShiftRows(state);
	DecSubBytes(state);

	int numberOfRounds = 9;

	for (int i = 9; i >= 1; i--) {
		AddSubRoundKey(state, expanded_key + (16 * i));
		InverseMixColumns(state);
		RightShiftRows(state);
		DecSubBytes(state);
	}

	AddSubRoundKey(state, expanded_key);

	for (int i = 0; i < 16; i++) {
		plaintext[i] = state[i];
	}
	return plaintext;
}

unsigned char* AES128::string2hex(string text, int n)
{
	unordered_map<char, int> mp;
	for (int i = 0; i < 10; i++)
	{
		mp[i + '0'] = i;
	}
	for (int i = 0; i < 6; i++)
	{
		mp[i + 'a'] = i + 10;
	}
	unsigned char* res = new unsigned char[n / 2];
	for (int i = 0; i < n / 2; i++)
	{
		char c1 = text.at(i * 2);
		char c2 = text.at(i * 2 + 1);
		int b1 = mp[c1];
		int b2 = mp[c2];
		res[i] = 16 * b1 + b2;
	}
	return res;
}

string AES128::hex2string(unsigned char* hex, int n)
{
	unordered_map<char, int> mp;
	for (int i = 0; i < 10; i++)
	{
		mp[i] = i + '0';
	}
	for (int i = 0; i < 6; i++)
	{
		mp[i + 10] = i + 'a';
	}
	string res;
	for (int i = 0; i < n; i++)
	{
		int x = hex[i];
		int b1 = mp[x / 16];
		int b2 = mp[x % 16];
		res += string(1, b1) + string(1, b2);
	}
	return res;
}
// Перевести std::string в hex
string AES128::stringToHex(const string& input) {
	stringstream ss;
	ss << std::hex << std::setfill('0');
	for (unsigned char c : input) {
		ss << setw(2) << static_cast<int>(c);
	}
    ss << "00";
	return ss.str();
}

// Перевести hex в std::string
string AES128::hexToString(const string& input) {
    string output;
    for (size_t i = 0; i < input.length(); i += 2) {
        string byte = input.substr(i, 2);
        char chr = static_cast<char>(stoul(byte, nullptr, 16));
        if (chr == '\0') break;
        output.push_back(chr);
    }
    return output;
}