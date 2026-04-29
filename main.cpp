#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

// Importamos nuestra cabecera
#include "SequentialFile.h"

int main() {
    std::cout << "=================================================\n";
    std::cout << "  STRESS TEST: SEQUENTIAL FILE (PAGINADO 4KB)  \n";
    std::cout << "=================================================\n\n";

    SequentialFile<int> db("datos.dat", "aux.dat", 50);

    const int NUM_RECORDS = 10000;

    std::cout << "Generando " << NUM_RECORDS << " claves aleatorias...\n";
    std::vector<int> keys;
    for (int i = 1; i <= NUM_RECORDS; ++i) {
        keys.push_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(keys.begin(), keys.end(), g);

    std::cout << "\n--- 1. INSERTANDO " << NUM_RECORDS << " REGISTROS ---\n";

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int k : keys) {
        Record<int> rec;
        rec.key = k;
        std::string dummy_data = "Data_Test_" + std::to_string(k);
        std::strncpy(rec.data, dummy_data.c_str(), sizeof(rec.data) - 1);

        db.add(rec);
    }

    db.rebuild();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Insercion completada con exito.\n";
    std::cout << "Tiempo total de ejecucion: " << duration_ms.count() << " ms\n\n";

    std::cout << "--- 2. PROBANDO BUSQUEDA PUNTUAL (search) ---\n";
    int search_target = keys[NUM_RECORDS / 2];
    std::cout << "Buscando la clave " << search_target << "...\n";

    start_time = std::chrono::high_resolution_clock::now();
    auto result = db.search(search_target);
    end_time = std::chrono::high_resolution_clock::now();
    auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "ENCONTRADO -> Clave: " << result.first.key << " | Datos: " << result.first.data << "\n";
    std::cout << "Costo de I/O empirico: " << result.second << " accesos a paginas.\n";
    std::cout << "Tiempo de consulta: " << duration_us.count() << " us\n\n";

    std::cout << "--- 3. PROBANDO BUSQUEDA POR RANGO (rangeSearch) ---\n";
    int range_start = NUM_RECORDS / 4;
    int range_end = range_start + 100;
    std::cout << "Buscando rango [" << range_start << " - " << range_end << "]...\n";

    start_time = std::chrono::high_resolution_clock::now();
    std::vector<Record<int>> range_results = db.rangeSearch(range_start, range_end);
    end_time = std::chrono::high_resolution_clock::now();
    duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Se encontraron " << range_results.size() << " registros.\n";
    std::cout << "Tiempo de busqueda por rango: " << duration_us.count() << " us\n\n";

    std::cout << "--- 4. PROBANDO ELIMINACION LOGICA (remove) ---\n";
    int delete_target = keys[10];
    std::cout << "Eliminando clave " << delete_target << "...\n";

    start_time = std::chrono::high_resolution_clock::now();
    db.remove(delete_target);
    end_time = std::chrono::high_resolution_clock::now();
    duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    std::cout << "Registro eliminado correctamente.\n";
    std::cout << "Tiempo de eliminacion: " << duration_us.count() << " us\n\n";

    std::cout << "=================================================\n";
    std::cout << "              FIN DEL STRESS TEST                \n";
    std::cout << "=================================================\n";

    return 0;
}