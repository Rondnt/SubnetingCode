#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cmath>

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
    int numSubnets;
    int maxHosts;
    char addSubnet;

    std::vector<Subnet> subnets;

    std::cout << "Ingrese dirección IP (en formato x.x.x.x): ";
    std::cin >> ip;

    std::cout << "Cantidad de subredes: ";
    std::cin >> numSubnets;

    std::cout << "Cantidad máxima de hosts por subred: ";
    std::cin >> maxHosts;

    int prefixLength = 32 - static_cast<int>(std::ceil(std::log2(maxHosts + 2))); // Calcula la longitud del prefijo

    do {
        Subnet subnet;

        clearBuffer(); // Limpiar el búfer de entrada

        std::cout << "Ingrese el nombre de la subred: ";
        std::getline(std::cin, subnet.name);

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
    std::cout << std::left << std::setw(20) << "SUBRED" << std::setw(30) << "IP FORMAT" << std::setw(20) << "NETWORK ADD" << std::setw(20) << "BROADCAST ADD" << "\n";

    std::string networkAdd = "192.168.10.0";
    int currentAddress = 0;

    for (const Subnet& subnet : subnets) {
        std::string broadcastAddress = calculateBroadcastAddress(networkAdd, subnet.size);
        std::string hostRangeStart = networkAdd;
        hostRangeStart.replace(hostRangeStart.rfind(".")+1, 3, std::to_string(currentAddress + 1));
        std::string hostRangeEnd = calculateBroadcastAddress(networkAdd, subnet.size - 1);
        
        std::cout << std::left << std::setw(20) << subnet.name << std::setw(30) << networkAdd << std::setw(20) << hostRangeStart << " Hasta " << hostRangeEnd << std::setw(20) << broadcastAddress << "\n";

        currentAddress += subnet.block;
        networkAdd = calculateBroadcastAddress(networkAdd, subnet.size + 1);
    }

    return 0;
}
