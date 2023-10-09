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

void clearBuffer() {
    char c;
    while (std::cin.get(c) && c != '\n') {
        // Leer y descartar caracteres del búfer hasta encontrar un salto de línea
    }
}

int main() {
    std::string ip;
    std::string subnetName;
    char addSubnet;

    std::vector<Subnet> subnets;

    std::cout << "Ingrese una dirección IP (en formato x.x.x.x o x.x.x.x/y): ";
    std::cin >> ip;

    do {
        Subnet subnet;

        clearBuffer(); // Limpiar el búfer de entrada

        std::cout << "Ingrese el nombre de la subred: ";
        std::getline(std::cin, subnetName);

        int prefixLength;
        std::cout << "Ingrese la longitud del prefijo (CIDR): ";
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
            subnet.name = subnetName; // Asignar el nombre aquí

            subnets.push_back(subnet);

        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Dirección IP o máscara inválida. Asegúrate de ingresar una dirección IP válida y una longitud de prefijo válida.\n";
            return 1;
        }

        std::cout << "¿Desea agregar otra subred? (y/n): ";
        std::cin >> addSubnet;
    } while (addSubnet == 'y' || addSubnet == 'Y');

    // Ordenar la tabla por el tamaño (size) en orden descendente
    std::sort(subnets.begin(), subnets.end(), [](const Subnet& a, const Subnet& b) {
        return a.size > b.size;
    });

    // Mostrar la tabla de subredes
    std::cout << "\nTabla de subredes:\n";
    std::cout << std::left << std::setw(20) << "SUBRED" << std::setw(10) << "SIZE" << std::setw(10) << "X" << std::setw(10) << "BLOCK" << std::setw(20) << "MASK" << "\n";

    for (const Subnet& subnet : subnets) {
        std::cout << std::left << std::setw(20) << subnet.name << std::setw(10) << subnet.size << std::setw(10) << subnet.x << std::setw(10) << subnet.block << std::setw(20) << subnet.mask << "\n";
    }

    return 0;
}
