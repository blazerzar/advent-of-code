#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <vector>
#include <utility>
#include <cassert>

struct Packet {
    int version;
    int type_ID;
    std::vector<Packet> content;
    long literal;
};

std::string read_data(const std::string &filename) {
    std::ifstream in; 
    in.open(filename);

    std::string line;
    std::getline(in, line);
    return line;
}

/* Transform each hex character to 4 binary bits */
std::string hex_to_bin(const std::string &hex_string) {
    std::ostringstream out;
    int a;
    for (char c : hex_string) {
        std::istringstream(std::string(1, c)) >> std::hex >> a;
        std::bitset<4> b(a);
        out << b;
    }
    
    return out.str();
}

/* Decapsulate one packet and return it as an object */
std::pair<Packet, int> decapsulate(const std::string &binary_packet) {
    // Parse version and type ID
    int version = stoi(binary_packet.substr(0, 3), 0, 2);
    int type_ID = stoi(binary_packet.substr(3, 3), 0, 2);
    Packet packet;
    packet.version = version;
    packet.type_ID = type_ID;

    if (type_ID == 4) {
        // Decode literal
        int i = 6;
        bool end = false;

        long long literal = 0ll;
        while (!end) {
            end = binary_packet[i] == '0';
            literal = (literal << 4) + stoi(binary_packet.substr(i + 1, 4), 0, 2);
            i += 5;
        }

        packet.literal = literal;
        return std::make_pair(packet, i);
    } else {
        // Decode operator packet
        int length_type_ID = binary_packet[6] - '0';

        if (length_type_ID == 0) {
            // Get length of content
            int length = stoi(binary_packet.substr(7, 15), 0, 2);
            int i = 22;

            while (i - 22 < length) {
                auto decapsulated = decapsulate(binary_packet.substr(i));
                packet.content.push_back(decapsulated.first);
                i += decapsulated.second;
            }

            return std::make_pair(packet, i);
        } else {
            // Get number of sub-packets
            int subpackets = stoi(binary_packet.substr(7, 11), 0, 2);
            int i = 18;

            for (int j = 0; j < subpackets; ++j) {
                auto decapsulated = decapsulate(binary_packet.substr(i));
                packet.content.push_back(decapsulated.first);
                i += decapsulated.second;
            }

            return std::make_pair(packet, i);
        }
    }
}

/* Return sum of version for given hierarchy of packets */
int versions_sum(const Packet &packet) {
    int versions = packet.version;
    if (packet.type_ID == 4) return versions;

    for (const Packet &p : packet.content) {
        versions += versions_sum(p);
    }

    return versions;
}

/* Return value that given packet represents */
long calculate_value(const Packet packet) {
    long result = 0;
    switch (packet.type_ID) {
        case 4:
            // Single number
            return packet.literal;
        case 0:
            // Sum of values
            for (Packet subpacket : packet.content) {
                result += calculate_value(subpacket);
            }
            return result;
        case 1:
            // Product of values
            result = 1;
            for (Packet subpacket : packet.content) {
                result *= calculate_value(subpacket);
            }
            return result;
        case 2:
            // Minimum of values
            result = calculate_value(packet.content[0]);
            for (decltype(packet.content.size()) i = 1; i < packet.content.size(); ++i) {
                long subpacket_value = calculate_value(packet.content[i]);
                if (subpacket_value < result) {
                    result = subpacket_value;
                }
            }
            return result;
        case 3:
            // Maximum of values
            result = calculate_value(packet.content[0]);
            for (decltype(packet.content.size()) i = 1; i < packet.content.size(); ++i) {
                long subpacket_value = calculate_value(packet.content[i]);
                if (subpacket_value > result) {
                    result = subpacket_value;
                }
            }
            return result;
        case 5:
            // Values of 1st sub-packet greater than 2nd
            return calculate_value(packet.content[0]) > calculate_value(packet.content[1]);
        case 6:
            // Value of 1st sub-packet less than 2nd
            return calculate_value(packet.content[0]) < calculate_value(packet.content[1]);
        case 7:
            // Value of 1st sub-packet equals 2nd
            return calculate_value(packet.content[0]) == calculate_value(packet.content[1]);
    }
    return -1;
}

void test() {
    Packet result = decapsulate(hex_to_bin("D2FE28")).first;
    assert(result.version == 6 && result.type_ID == 4 && result.literal == 2021);

    result = decapsulate(hex_to_bin("38006F45291200")).first;
    assert(result.version == 1 && result.type_ID == 6 && result.content.size() == 2);
    assert(result.content[0].literal == 10 && result.content[1].literal == 20);

    result = decapsulate(hex_to_bin("EE00D40C823060")).first;
    assert(result.version == 7 && result.type_ID == 3 && result.content.size() == 3);
    assert(result.content[0].literal == 1 && result.content[1].literal == 2 && result.content[2].literal == 3);

    result = decapsulate(hex_to_bin("8A004A801A8002F478")).first;
    assert(versions_sum(result) == 16);
    result = decapsulate(hex_to_bin("620080001611562C8802118E34")).first;
    assert(versions_sum(result) == 12);
    result = decapsulate(hex_to_bin("C0015000016115A2E0802F182340")).first;
    assert(versions_sum(result) == 23);
    result = decapsulate(hex_to_bin("A0016C880162017C3686B18A3D4780")).first;
    assert(versions_sum(result) == 31);

    result = decapsulate(hex_to_bin("C200B40A82")).first;
    assert(calculate_value(result) == 3);
    result = decapsulate(hex_to_bin("04005AC33890")).first;
    assert(calculate_value(result) == 54);
    result = decapsulate(hex_to_bin("880086C3E88112")).first;
    assert(calculate_value(result) == 7);
    result = decapsulate(hex_to_bin("CE00C43D881120")).first;
    assert(calculate_value(result) == 9);
    result = decapsulate(hex_to_bin("D8005AC2A8F0")).first;
    assert(calculate_value(result) == 1);
    result = decapsulate(hex_to_bin("F600BC2D8F")).first;
    assert(calculate_value(result) == 0);
    result = decapsulate(hex_to_bin("9C005AC2F8F0")).first;
    assert(calculate_value(result) == 0);
    result = decapsulate(hex_to_bin("9C0141080250320F1802104A08")).first;
    assert(calculate_value(result) == 1);
}

int main() {
    test();

    std::string data = read_data("inputs/input16.txt");
    Packet packet = decapsulate(hex_to_bin(data)).first;    
    std::cout << versions_sum(packet) << "\n";
    std::cout << calculate_value(packet) << "\n";
    
    return 0;
}
