**Ingeniería de Software (3.4.214)** 

# **Trabajo Práctico Obligatorio (TPO)** 

Implementación de un Ajedrez 

## 1. Introducción 

Este trabajo integra, sobre un proyecto real y acotado, los principios de arquitectura y diseño vistos a lo largo de la cursada bajo el eje de “Ingenio y Diseño”: separación entre núcleo y adaptadores, SOLID, inyección de dependencias, composición sobre herencia, y las tres preguntas que atraviesan cada decisión — qué, por qué, y cuándo romperlo. 

## 2. Consigna general 

- El proyecto consiste en implementar el juego de ajedrez. 

- El sistema debe ser completamente modular, flexible y escalable. 

No se indica qué técnicas, principios o patrones puntuales utilizar para lograrlo. Durante la cursada vimos principios, patrones y una corriente de pensamiento — no la resolución de este problema en particular. Es la misma lógica que en matemática: se enseña a sumar, restar, multiplicar, dividir, qué es un polinomio o una potencia, y después se pide resolver una integral con las herramientas que cada uno considere adecuadas. Acá aplica el mismo criterio. 

## 3. Alcance obligatorio 

- Tablero de 8x8. 

- Las 6 piezas (peón, torre, caballo, alfil, reina, rey), cada una con su movimiento propio y válido. 

- Alternancia de turnos entre ambos jugadores. 

- Captura de piezas. 

- Validación de movimientos inválidos: el sistema no debe permitir mover una pieza a una posición prohibida por sus propias reglas. 

- Detección de jaque: el sistema debe poder identificar cuándo un rey queda en jaque. 

## 4. Alcance opcional 

No forma parte de los requisitos obligatorios. No resta si no está. Si un grupo decide sumarlo, es una buena oportunidad para mostrar patrones adicionales (Strategy, State, Command, Observer, entre otros) — sin que eso haya sido pedido explícitamente. 

- Jaque mate. 

- Enroque. 

- Captura al paso. 

- Promoción de peón. 

- Condiciones de tablas (ahogado, repetición, regla de los 50 movimientos). 

- Oponente con inteligencia artificial. 

- Deshacer / rehacer movimientos. 

- Interfaz gráfica más allá de lo mínimo necesario para jugar. 

## 5. Requisitos de arquitectura 

- El núcleo (las reglas del juego) no debe depender de ningún framework, motor gráfico ni infraestructura externa. 

- El núcleo debe poder probarse con tests unitarios sin levantar ninguna infraestructura. 

- La arquitectura debe permitir agregar clases nuevas sin modificar las existentes, sin romper relaciones de herencia, y sin desarmar la composición general del sistema. 

- El diagrama de clases (UML) debe reflejar fielmente la arquitectura real, y mantenerse consistente con el código durante todo el desarrollo — no es un diagrama aspiracional hecho una sola vez al principio. 

## 6. Entregables 

- Código fuente completo. 

- Diagrama de clases (UML). 

- Tests unitarios del núcleo. 

- Documento breve de justificación de decisiones de diseño, en formato qué / por qué / cuándo se romperían esas decisiones — aplicado a las elecciones propias del grupo, no a las de la cátedra. 

- Presentación (PPTX) para la exposición grupal en clase. 

## 7. Formación de grupos 

Grupos de 3 a 5 personas. 

## 8. Instancias de evaluación 

**Entrega y exposición grupal (PPTX, en clase):** da la nota grupal, que integra la nota de cursada junto con los parciales. 

**Defensa individual:** es el examen final reglamentario de la materia. Cada estudiante defiende individualmente el mismo proyecto grupal, y puede incluir una consigna de extensión no programada por el grupo (por ejemplo, agregar una pieza nueva o cambiar el tamaño del tablero) para evaluar en el momento qué tan escalable es realmente la arquitectura construida. 

## 9. Criterios de evaluación 

|**Categoría**|**Qué se evalúa**|**Instancia**|
|---|---|---|
|**Cumplimiento del**<br>**alcance obligatorio**|Tablero, 6 piezas con movimiento propio, turnos, captura,<br>validación de movimientos inválidos, detección de jaque.|Grupal|
|**Arquitectura y diseño**|Separación núcleo/adaptador, SOLID, composición sobre<br>herencia, modularidad. Se evalúa el código real, no solo el<br>diagrama.|Grupal|
|**Diagrama de clases**|Consistencia entre el diagrama (UML) y el código entregado —<br>no debe ser un diagrama aspiracional desconectado de la<br>implementación.|Grupal|
|**Tests del núcleo**|Cobertura razonable de la lógica de negocio, corridos sin<br>ninguna infraestructura externa.|Grupal|
|**Documento de**<br>**justificación**|Decisiones de diseño explicadas en formato qué / por qué /<br>cuándo se romperían, aplicado a las decisiones propias del<br>grupo.|Grupal|
|**Exposición (PPTX)**|Claridad al comunicar la arquitectura y las decisiones tomadas.|Grupal|



|**Defensa individual**|Comprensión personal del diseño. Puede incluir una extensión|Individual —|
|---|---|---|
||no programada por el grupo (ej. una pieza nueva, otro tamaño|examen final|
||de tablero) para verificar en el momento qué tan escalable es la<br>arquitectura.||



