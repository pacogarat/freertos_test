/**
 * Sin probar, quitar comentario cuando se haya probado
 **/
 #ifndef pinMode
    #include <Arduino.h>
#endif

class Nema17 {
    public:
        Nema17(int pinDir, int pinStep);
        void setup();
        void fullRev(int loops = 1, int forward = true);
    private:
        int pinDir;
        int pinStep;
        const int  steps_per_rev = 200;
};