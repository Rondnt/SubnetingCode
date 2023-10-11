#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>

struct Subnet {
    std::string name;
    int size;
    int x;
    int block;
    std::string mask;
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

std::string calculateSubnetMask(int prefixLength) {
    std::string mask = "";
    int remainingBits = prefixLength;
    int octet = 0xFF;  // Start with 8 bits set to 1.

    while (remainingBits >= 8) {
        mask += std::to_string(octet);
        mask += ".";
        remainingBits -= 8;
    }

    // Calculate the last octet if there are remaining bits.
    if (remainingBits > 0) {
        int lastOctetMask = (0xFF << (8 - remainingBits)) & 0xFF;
        mask += std::to_string(lastOctetMask);
    } else {
        // Remove the trailing dot.
        mask.pop_back();
    }

    return mask;
}

std::string calculateNetworkAddress(const std::string& ip, int prefixLength) {
    std::string maskedIp = applySubnetMask(ip, 32 - prefixLength);
    return maskedIp;
}

std::string calculateBroadcastAddress(const std::string& networkAddress, int prefixLength) {
    std::string broadcastAddress = networkAddress;

    // Calculate the last octet if there are remaining bits.
    int remainingBits = prefixLength - (32 - networkAddress.size()) + 1;
    if (remainingBits > 0) {
        int lastOctetMask = (0xFF >> (8 - remainingBits)) & 0xFF;
        int lastOctet = std::stoi(networkAddress.substr(networkAddress.size() - 3));
        int broadcastOctet = lastOctet | lastOctetMask;
        broadcastAddress = networkAddress.substr(0, networkAddress.size() - 3) + std::to_string(broadcastOctet);
    }

    return broadcastAddress;
}

void clearBuffer() {
    char c;
    while (std::cin.get(c) && c != '\n') {
        // Leer y descartar caracteres del búfer hasta encontrar un salto de línea
    }
}

int main() {
    std::string ip;
    char addSubnet;

    std::vector<Subnet> subnets;

    std::cout << "Ingrese una dirección IP (en formato x.x.x.x o x.x.x.x/y): ";
    std::cin >> ip;

    do {
        Subnet subnet;

        clearBuffer(); // Limpiar el búfer de entrada

        std::cout << "Ingrese el nombre de la subred: ";
        std::getline(std::cin, subnet.name);

        int prefixLength;
        std::cout << "Ingrese la longitud del prefijo (CIDR) para la subred " << subnet.name << ": ";
        std::cin >> prefixLength;

        try {
            subnet.size = prefixLength;

            // Calcular 'x' y el bloque
            int x = 0;
            while (true) {
                if ((1 << x) >= (prefixLength + 2)) {
                    break;
                }
                x++;
            }
            subnet.x = x;
            subnet.block = 1 << x;

            // Calcular la máscara como "32 - x"
            int mask = 32 - x;
            subnet.mask = "/" + std::to_string(mask) + " (" + calculateSubnetMask(mask) + ")";

            subnets.push_back(subnet);

        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Longitud del prefijo inválida. Asegúrate de ingresar una longitud de prefijo válida.\n";
            return 1;
        }

        std::cout << "¿Desea agregar otra subred? (y/n): ";
        std::cin >> addSubnet;
    } while (addSubnet == 'y' || addSubnet == 'Y');

    // Ordenar la tabla de subredes por tamaño (size) en orden descendente
    std::sort(subnets.begin(), subnets.end(), [](const Subnet& a, const Subnet& b) {
        return a.size > b.size;
    });

    // Mostrar la tabla de subredes
    std::cout << "\nTabla de subredes:\n";
    std::cout << std::left << std::setw(20) << "SUBRED" << std::setw(10) << "SIZE" << std::setw(10) << "X" << std::setw(10) << "BLOCK" << std::setw(20) << "MASK" << "\n";

    for (const Subnet& subnet : subnets) {
        std::cout << std::left << std::setw(20) << subnet.name << std::setw(10) << subnet.size << std::setw(10) << subnet.x << std::setw(10) << subnet.block << std::setw(20) << subnet.mask << "\n";
    }

    // Mostrar la tabla de cálculo de Direcciones de Red y Broadcast
    std::cout << "\nTabla de cálculo de Direcciones de Red y Broadcast:\n";
    std::cout << std::left << std::setw(20) << "SUBRED" << std::setw(20) << "IP FORMAT" << std::setw(20) << "NETWORK ADD" << std::setw(20) << "BROADCAST ADD" << "\n";

    std::string ipFormat = "172.00010000.00000000."; // Prefix for IP Format
    std::string networkAdd = "172.16.0.";
    int currentAddress = 0;

    for (const Subnet& subnet : subnets) {
        std::string ipFormatLine = ipFormat;
        std::string networkAddLine = networkAdd + std::to_string(currentAddress);

        for (int i = 0; i < subnet.x; i++) {
            ipFormatLine += "1";
        }

        for (int i = subnet.x; i < 6; i++) {
            ipFormatLine += "0";
        }

        int broadcast = currentAddress + subnet.block - 1;
        std::string broadcastAddLine = networkAdd + std::to_string(broadcast);

        std::cout << std::left << std::setw(20) << subnet.name << std::setw(20) << ipFormatLine << std::setw(20) << networkAddLine << std::setw(20) << broadcastAddLine << "\n";

        currentAddress += subnet.block;
    }

    return 0;
}
