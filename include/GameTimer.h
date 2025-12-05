#include <cstdio>

class GameTimer {

    private:

        float timer;     

    public:

        float interval;     

        GameTimer(float interval = 1000.0f) : timer{0.0f}, interval{interval} {}

        bool update(float deltaTime) {
            
            timer += deltaTime;

            if (timer >= interval) {
                
                timer -= interval; 
            
                return true;

            }

            return false;

        }

};