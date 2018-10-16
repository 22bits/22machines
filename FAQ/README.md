# 22machines F.A.Q.

### ¿Qué es un módulo?

Usualmente, los aparatos electrónicos de uso general son _standalone_, es decir, que en si mismos cumplen todas las funciones para los cuales fueron diseñados, como un notebook. Un computador de escritorio, en cambio, requiere de muchas partes para funcionar completamente (CPU, pantalla, teclado, mouse, etc.), sin embargo, no son sistemas cerrados, y podemos elegir qué tipo de pantalla, mouse, teclado o CPU queremos, o incluso modificar internamente las partes de esta última. Esto quiere decir que es modular, sus partes son intercambiables y aceptables según el uso que necesitemos.

Un sintetizador modular es lo mismo. Desde hace años existe un estándar llamado Eurorack, el cuál consiste en módulos de generación y modificación del sonido, donde los fabricantes diseñan con ciertos criterios electrónicos y dimensionales, para que puedan ser compatibles con los de otra marca.

### ¿Por qué una alternativa al Eurorack?

El estándar Eurorack es abierto, cualquiera puede diseñar y fabricar partes y distribuirlas según ese estándar. Lamentablemente, al provenir de Europa, con un nivel de acceso económico e industrial distinto al latinoamericano, fue pensado con componentes y partes de fácil acceso allá, pero no tanto por este lado. Nuestros módulos están armados exclusivamente con componentes que encontramos en las tiendas de Santiago de Chile, y en la medida que conozcamos la situación de otras regiones del continente, lo iremos expandiendo y acomodando a ellas. De todas formas, también son compatibles con el sistema Eurorack.

### ¿Qué es un trigger?

Es una pequeña señal de 5 voltios (cuyo símbolo es V), de aproximadamente 15 milisegundos (ms). En electrónica digital, también se entiende como un "1" lógico. De forma muy simplificada, cada vez que presionas un botón en un dispositivo electrónico (un click en mouse, por ejemplo), estás enviando un "1", que la máquina receptora entiende y convierte en una acción en el procesador, y luego en la pantalla. 

Esta señal no necesariamente debe provenir de una acción mecánica (un botón), sino que se puede automatizar, por ejemplo con secuenciadores, sensores o recogiéndolos de la big data. ¡Las posibilidades son muchas!

### ¿Qué es un servo motor?

Es un tipo de actuador, usado frecuentemente en robótica, que genera movimiento a través de engranajes. Existen de varios tipos, voltajes y fuerzas, pero casi todos tienen 3 pines de entrada desde los cuales se controlan:

> 1. Ground o tierra 
> 2. Alimentación (usualmente 5V)
> 3. Señal

Nosotros utilizamos motores de 5V, y de 180º. 

### ¿Cómo se controla un servo?

Con una señal parecida a un trigger, pero ligeramente más compleja, denominada PWM (Pulse-Width Modulation, o modulación por ancho de pulso en español). Es un trigger que es enviado repetidamente al servomotor cada 20ms. Según que tan ancho sea este trigger, el motor se ubica en una posición de 0º a 180º (o -90º a +90º), como la imagen adjunta. Si este pulso dura 1ms (1000us), el servo se ubica en -90º, si este dura 2 ms (2000us), va a la posición +90º, por lo que cualquier posición a la que queramos ir entre medio debe ser de pulsos precisos entre 1ms y 2 ms de duración, cada 20ms. Como es muy difícil de realizar de forma humana, se utilizan controladores especiales para manejar los servos, y Arduino es uno de ellos. Este incluye una librería para manejarlos de forma sencilla.

![Image of Servos](https://github.com/22bits/22machines/blob/master/FAQ/images/servos.JPG) 
>  ###### By <a href="//commons.wikimedia.org/w/index.php?title=User:Hforesti&amp;action=edit&amp;redlink=1" class="new" title="User:Hforesti (page does not exist)">Hforesti</a> - <span class="int-own-work" lang="en">Own work</span>, <a href="https://creativecommons.org/licenses/by-sa/4.0" title="Creative Commons Attribution-Share Alike 4.0">CC BY-SA 4.0</a>, <a href="https://commons.wikimedia.org/w/index.php?curid=3705164">Link</a>

### ¿Qué es Arduino?

Arduino es un pequeño micro controlador de código abierto, el cuál permite programar instrucciones sencillas en el computador, las cuales son realizadas por la placa de desarrollo, como mover motores, generar triggers, leer botones, encender leds, etc. Existen diversas versiones de la placa, que se diferencian en sus dimensiones y número de pines, pero todas se programan de la misma forma y con el mismo software.

### ¿Cómo consigo un módulo de 22machines?

Por ahora, solo estamos distribuyendo la C05-22mini a través de talleres presenciales, gratuitos o pagados, según la institución que nos apoye. Nos interesa generar comunidad, diálogos sobre tecnología y experimentación de forma presencial. Sígue a Colectivo 22bits en [Facebook](https://www.facebook.com/colectivo22bits/) o [Instagram](http://instagram.com/colectivo22bits) para enterarte de la realización de los talleres.

### ¿Cómo se financian?

Trabajando de manera freelance o part-time en proyectos similares a nuestra línea de trabajo (o no tanto), y a través de instituciones que nos convocan a realizar talleres, charlas, instalaciones o conciertos. Si trabajas o diriges una de ellas, no dudes en escribirnos a hola@22bits.org para ver como podemos colaborar 😌

