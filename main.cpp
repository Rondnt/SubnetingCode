#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include <stdexcept>

struct Subnet {
    std::string name;
    std::string network;
    std::string broadcast;
};

std::string applySubnetMask(const std::string& ip, int prefixLength) {
    std::istringstream iss(ip);
    std::string token;
    std::string result;

    while (std::getline(iss, token, '.')) {
        int octet = std::stoi(token);
        int subnetBits = prefixLength >= 8 ? 8 : prefixLength;
        int subnetMask = 255 << (8 - subnetBits);
        result += std::to_string(octet & subnetMask);
        result += ".";
        prefixLength -= subnetBits;
    }

    result.pop_back();  // Remove the trailing '.'

    return result;
}

int main() {
    std::string ip;
    char addSubnet;

    std::vector<Subnet> subnets;

    do {
        std::cout << "Ingrese una dirección IP (en formato x.x.x.x o x.x.x.x/y): ";
        std::cin >> ip;

        Subnet subnet;

        std::cout << "Ingrese el nombre de la subred: ";
        std::cin >> subnet.name;

        int prefixLength;
        std::cout << "Ingrese la longitud del prefijo (CIDR): ";
        std::cin >> prefixLength;

        try {
            subnet.network = applySubnetMask(ip, prefixLength);

            // Calcular la dirección de broadcast
            std::bitset<32> bits(std::stoul(subnet.network, nullptr, 0));
            for (int i = 31; i >= 31 - (32 - prefixLength); --i) {
                bits[i] = 1;
            }
            subnet.broadcast = std::to_string(static_cast<unsigned long>(bits.to_ulong()));
        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Dirección IP o máscara inválida. Asegúrate de ingresar una dirección IP válida y una longitud de prefijo válida.\n";
            return 1;
        }

        subnets.push_back(subnet);

        std::cout << "¿Desea agregar otra subred? (y/n): ";
        std::cin >> addSubnet;
    } while (addSubnet == 'y' || addSubnet == 'Y');

    // Mostrar la tabla de subredes
    std::cout << "\nTabla de subredes:\n";
    std::cout << "Nombre\t\tRed\t\tBroadcast\n";
    for (const Subnet& subnet : subnets) {
        std::cout << subnet.name << "\t\t" << subnet.network << "\t" << subnet.broadcast << "\n";
    }

    return 0;
}
