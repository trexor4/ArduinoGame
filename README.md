# ArduinoGame
Game using arduino final for ENGR 103
This game using buttons will make you able to switch between the levels and help you keep track of score consisting of four stages that you can navigate by pressing the right and left buttons. The game will keep score and whenever you beat a level it will add to the score by 1 and so on and so forth. The first stage is using the dice function we are given and is completely up to chance if you get a 6 then you get a point along with giving you a nice beep. From there you can navigate to the next level by pressing the right button and that will take you to the shake-o-meter. Basically using a map function the harder you shake the more LEDS will be lit up and when all 10 are lit up you get a point along with giving you a nice beep. Finally you can travel to the final level by pressing the right button but at any time you can go back to the previous levels to just earn more points if you want or if you don't have enough points. The final level is a sound meter and as long as you yell loud enough it will use the map function and light up that amount of LEDS and once you get all 10 you get another point along with giving you a nice beep. Once you have enough points and you have tried to press the right button to proceed to the final stage it will print a you have won message and make a celebratory ear deafening beep (:
//Inputs:
Accelerometer sensor
Sound sensor
//Outputs:
LEDs
Sound box
//Map functions:
Mapping sound level to LED light #
Mapping Accelerometer to LED light #
