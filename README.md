# Conversor-CC-CC-Buck-Boost-Interleaved
O conversor CC-CC dimensionado neste projeto, poderá ser inserido num sistema de carga/descarga
de baterias, como por exemplo de um veículo elétrico, onde funcionará como conversor elevador
durante a descarga e conversor abaixador durante a carga, pois este conversor será projetado para
apresentar a melhor eficiência energética, tendo como objetivo a redução de perdas nos
semicondutores e elementos armazenadores de energia, no que sucede um menor desgaste das
baterias.

Com a realização deste projeto foi possível desenvolver um conversor CC-CC bidirecional, capaz de
atuar como um conversor abaixador num sentido e como um conversor elevador no sentido contrário.
Este conversor tem como objetivo, elevar ou baixar o valor da tensão de saída face a um valor da
tensão de entrada, i.e., em função do que é solicitado pelo sistema onde está inserido, sendo
controlado por uma corrente de referência definida pelo sistema onde poderá ser inserido. Este
conversor permite gerar uma corrente (de saída em modo buck ou de entrada em modo boost) com
o menor valor de ripple possível, minimizando assim as perdas nos semicondutores e nos elementos
armazenadores de energia.

Este conversor permite ser inserido num sistema com uma tensão nominal de entrada ou saída de
200 V, permite operar como conversor abaixador (buck-mode) ou elevador (boost-mode) e permite
uma corrente nominal de 16 A (na saída como conversor abaixador ou na entrada como conversor
elevador). O conversor desenvolvido permite uma frequência máxima de operação de 20 kHz e é
controlado digitalmente pela plataforma de desenvolvimento STM32. O conversor deve operar na
vertical, sem obstrução do dissipador, de modo a facilitar a troca de calor entre o dissipador e o meio
ambiente.
# Esquemático geral do sistema
![alt text](https://github.com/SirBlaze10/Conversor-CC-CC-Buck-Boost-Interleaved/blob/main/Fotos%20readme/esquematico.png)

# Fluxograma da rotina de controlo
![alt text](https://github.com/SirBlaze10/Conversor-CC-CC-Buck-Boost-Interleaved/blob/main/Fotos%20readme/fluxograma_V2.jpg)

# Prototipo desenvolvido
![alt text](https://github.com/SirBlaze10/Conversor-CC-CC-Buck-Boost-Interleaved/blob/main/Fotos%20readme/Prototipo.png)
