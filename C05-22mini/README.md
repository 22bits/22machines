# C05-22mini
(tutumini)

##### Diseñada por Colectivo 22bits (http://22bits.org)
##### Bárbara Molina & Matías Serrano
##### Programada por Elías Zacarías (http://blog.damnsoft.org/)
___________________________________

22mini es una drum machine mecánica de 2 pistas y 8 pasos, donde cada relación entra paso y pista puede activar un servo motor en una acción percusiva.

<iframe width="560" height="315" src="https://www.youtube.com/embed/PP0vCVJRCfk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Especificaciones (1a versión):

    - 2 pistas con salidas individuales
    - Hasta 8 pasos por pista
    - Salidas pueden ser para controlar servo motores o generar triggers de 5v
    - Control de tempo
    - Control de Play/Pause/Clear
    - 2 pantallas de visualización, 
    - Entrada y salida de Sync no dependientes 
    - Cada máquina puede seleccionar si obedece al pulso externo o al interno a través de un switch
    - Operación a través de 9~12V con centro positivo
    
### Descripción de funcionamiento
  
22mini está programada en Arduino, la cual lee la información ingresada a través de los botones, lo muestra en la pantalla de LEDS, y activa los servomotores según la secuencia. La velocidad de ésta es dependiente de la perilla de tempo o de la entrada de Sync, que avanza un paso a cada lectura de pulso en Sync IN.

Electrónicamente, todos los componentes están en función del Arduino. 8 de los 9 botones ingresan a través de un circuito integrado CD4021 (PISO; Parallel IN, Serial OUT), el cuál permite leer 8 entradas paralelas utilizando solo tres pines del Arduino. El noveno botón ingresa directamente a éste. Los botones de la columna izquiera permiten seleccionar qué pista quiero escribir, y los de la fila inferior en qué paso quiero activar la secuencia. El botón de OFFBEAT permite visualizar entre pasos impares (1, 3, 5 y 7) y pares (2, 4, 6, 8). MUTE, al mantenerse presionado, permite silenciar una pista o pasos, dejando la oportunidad de generar patrones de 1/8 hasta 8/8. El botón de Play permite pausar o reproducir la secuencia, y al mantenerse presionado limpia todas las secuencias.
    
Las luces LED son controladas a través de un circuito integrado MAX4219, el cuál está diseñado para manejar matrices LED de hasta 8x8. Está sobreestimado, ya que solo maneja 17 leds de 64 posibles. La razón será explicada más adelante, en la sección de modificaciones. 

El tempo es controlado por pulsos de 5v y 15ms. Cada vez que Arduino recibe uno de estos pulsos, entiende que debe avanzar un paso la secuencia. Este pulso puede provenir internamente, obedeciendo al potenciómetro de TEMPO, o de forma externa, obedeciendo a la entrada SYNC IN. Este permite sincronizar 22minis entre ellas, o con otros secuenciadores (como los Korg, por ejemplo). Cada 22mini copia en la salida Sync OUT el tempo al cuál está avanzando. El Switch SYNC EXT/INT permite que la secuencia avance según el tempo externo o interno a disposición, evitando que cada 22mini sea esclava de la entrada de sync, y pueda independizase de esta, generando nuevos ritmos complejos.

Las salidas están predeterminadas para controlar servomotores en un movimiento percusivo. Practicamente cualquier servomotor de 5v y 180º es posible de activar, siempre y cuando no supere 1A de consumo. Cada salida de servo es alimentada por un único regulador 7805, por lo que la limitación de amperaje depende de éste. 

La alimentación del circuito puede ser de 9 a 12V, con un amperaje de 1A para motores más pequeños (como el SG09)
    
### Modificaciones sugeridas
    
   - 22mini está programada exactamente con el mismo código que C01-SEQ, solo varía a través de la línea "#define _22_MINI". Si está comentada, asignará los botones y leds para la versión de 4 pistas y 16 pasos, lo cual puede generar problemas para una 22mini.
   
   - Si se comenta la línea "#define SERVO_OUTPUT", las salidas pasan a ser de triggers de 5V, lo que permitiría utilizar estas para activar otro tipo de actuadores, usarlos como sync para otros secuenciadores, etc. Además cada conector cuenta con 5v (hasta 1A) y un GND.
   
   - La pantalla puede ser re-programada para que cuente hasta 16 pasos, y así tener más posibilidades rítmicas. Se sugiere utilizar dos horizontales para este conteo. Esta modificación no ha sido programada aún.
   
   - Hay pads de la PCB que no incluyen componentes, los cuales están abiertos para agregar variaciones. Estos son 4 pines análogos (A4, A5, A6, A7), uno digital (D3), D10 y D11 para salidas no definidas en el código, TX y RX.  

### Sugerencias al ensamblar:
  
   - D23 y R19 reducen el voltaje de la entrada de SYNC, no deben soldarse.
   - El emisor de Q1 debe ir a GND, por lo que se debe puentear al disipador.
   - Los Condensadores electrolíticos deben ir horizontales para que no interfieran espacialmente con la placa superior. Pueden ir soldados hacia arriba o hacia abajo según criterio de la persona que lo ensamble.
   - Soldar los headers utilizando los separadores como guías, para hacer más sencillo el montaje y desmontaje de las dos placas
    
    
    
 _____________________________
 
C05 - 22mini por Bárbara Molina & Matías Serrano se distribuye bajo una Licencia Creative Commons Atribución-NoComercial-CompartirIgual 4.0 Internacional.
 
Colectivo 22bits promueve cualquier tipo de modificación y hackeo a la máquina, no así el uso comercial que se le pueda dar sin expresa autorización de sus creadores. Si te fabricas una, la utilizas en tu set, o la usas para algo nuevo, haznos saber y te compartiremos en nuestras redes sociales (si te interesa) :)
