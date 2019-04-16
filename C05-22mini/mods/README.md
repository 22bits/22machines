# Modificaciones a 22mini
____________________________________

## Mover otro tipo de motores

La salida de 22mini está diseñada para mover servomotores, lo que realiza el arduino, generando una señal [PWM](https://en.wikipedia.org/wiki/Servo_control) que cambia el ángulo del Servomotor. Para mover otro tipo de motores que operen con señales 5v DC, como Motores DC, bombas de agua, vibradores, ventiladores, etc., se requiere un transistor que le de la potencia necesaria para que opere el motor

1) Comentar la línea (poner "//" antes) en Arduino, que indica Servo Output

 ![image](https://github.com/22bits/22machines/blob/master/images/22minimod1.png)
 
 2) Armar el siguiente circuito dos veces con un IRFZ48N o cualquier MOSFET canal-n*, un diodo 1n4007, y una resistencia de 220. 
 
 ![image](https://github.com/22bits/22machines/blob/master/images/22minimod2.png)
 
 3) Conectar la salida de cada servo a la entrada de señal del circuito antes ensamblado, teniendo cuidado de respetar la polaridad indicada.
 
 4) Conectar tu motor en el OUTPUT

 5) Listo! 22mini moverá tu motor con pequeños pulsos. Para modificar el tiempo de éste, puedes utilizar un circuito con un chip 555 en su configuración monoestable.
 
 *Acá hay un listado en la tienda HMTECHTRONIC en Santiago con transistores. Cualquier MOSFET canal N de más de 2A funciona.
