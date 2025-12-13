#pragma once

namespace Qul {

    // Plantilla simple que simula Qul::ListModel
    template <typename T>
    class ListModel {
    public:
        virtual ~ListModel() {}
        
        // Métodos virtuales puros que espera la implementación real
        virtual int count() const = 0;
        virtual T data(int index) const = 0;
        
        // Simulación de la señal de "reset" (no hace nada en el test)
        void modelReset() {}
    };

}