#define SCK 52
#define MOSI 51
#define MISO 50
#define CS 53
// Definindo o endereço dos bancos - Página 14 [ENC 28J60]
// Bank 0
#define ERDPTL 0x00
#define ERDPTH 0x01
#define EWRPTL 0x02
#define EWRPTH 0x03
#define ETXSTL 0x04
#define ETXSTH 0x05
#define ETXNDL 0x06
#define ETXNDH 0x07
#define ERXSTL 0x08
#define ERXSTH 0x09
#define ERXNDL 0x0A
#define ERXNDH 0x0B
#define ERXRDPTL 0x0C
#define ERXRDPTH 0x0D
#define ERXWRPTL 0x0E
#define ERXWRPTH 0x0F
#define EDMASTL 0x10
#define EDMASTH 0x11
#define EDMANDL 0x12
#define EDMANDH 0x13

#define EDMADSTL 0x14
#define EDMADSTH 0x15
#define EDMACSL 0x16
#define EDMACSH 0x17

// Bank 1
#define EHT0 0x00
#define EHT1 0x01
#define EHT2 0x02
#define EHT3 0x03
#define EHT4 0x04
#define EHT5 0x05
#define EHT6 0x06
#define EHT7 0x07
#define EPMM0 0x08
#define EPMM1 0x09
#define EPMM2 0x0A
#define EPMM3 0x0B
#define EPMM4 0x0C
#define EPMM5 0x0D
#define EPMM6 0x0E
#define EPMM7 0x0F
#define EPMCSL 0x10
#define EPMCSH 0x11
#define EPMOL 0x14
#define EPMOH 0x15
#define ERXFCON 0x18
#define ERKTCNT 0x19

// Bank 2
#define MACON1 0x00
#define MACON3 0x02
#define MACON4 0x03
#define MABBIPG 0x04
#define MAIPGL 0x06
#define MAIPGH 0x07
#define MACLCON1 0x08
#define MACLCON2 0x09
#define MAMXFLL 0x0A
#define MAMXFLH 0x0B
#define MICMD 0x12
#define MIREGADR 0x14
#define MIWRL 0x16
#define MIWRH 0x17
#define MIRDL 0x18
#define MIRDH 0x19

// Bank 3
#define MAADR5 0x00
#define MAADR6 0x01
#define MAADR3 0x02
#define MAADR4 0x03
#define MAADR1 0x04
#define MAADR2 0x05
#define EBSTSD 0x06
#define EBSTCON 0x07
#define EBSTCSL 0x08
#define EBSTCSH 0x09
#define MISTAT 0x0A
#define EREVID 0x12
#define ECOCON 0x15
#define EFLOCON 0x17
#define EPAUSL 0x18
#define EPAUSH 0x19

// Bank Geral
#define EIE 0x1B
#define EIR 0x1C
#define ESTAT 0x1D
#define ECON2 0x1E
#define ECON1 0x1F

// Registrador LED
#define PHLCON 0x14

// Definindo os OPCODES - Página 28 [ENC 28J60]
#define OPRCR 0x00
#define OPRBM 0x20
#define OPWCR 0x40
#define OPWBM 0x60
#define OPBFS 0x80
#define OPBFC 0xA0
#define OPSRC 0xE0


// CONFIGURANDO COMO MESTRE OU ESCRAVO - Página 136 [ATMega 360p]
void config_spi() {
  pinMode(SCK, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(CS, OUTPUT);

  // DEIXANDO A COMUNICAÇÃO FECHADA
  digitalWrite(CS, 1);

  // CONFIGURANDO O SPCR (MENSAGEIRO) - Página 140 [ATMega 360p]
  // BIT 7: Interrupção habilitada (1) /desabitada (0)
  // BIT 6: Ativar SPI (1) / Desativar SPI (0)
  // BIT 5: Se você quer que enviar primeiro o bit mais significativo (0) / menos significativo (1)
  // BIT 4: Define se o ARTMEGA será Mestre (1) ou Escravo (0)
  // BIT 3|2: Define borda de subida e descida
  // BIT 1|0: Define a frequência de clock do ARTMEGA

  SPCR = 0b01010000;
}


// PARA OPERAÇÃO DE ESCRITA SÃO USADOS 02 (DOIS) BYTES, ASSIM, DEVE SER FEITO DOIS ENVIOS

// ESCREVENDO O OPCODE + ADRESS (PRIMEIRO ENVIO)
void write_register(unsigned char opcode, unsigned char adress, unsigned char data) {
  digitalWrite(CS, 0);     // ABAIXANDO O NÍVEL DO CS BAIXO PARA INICIAR A COMUNICAÇÃO
  SPDR = opcode | adress;  // CONCATENANDO O OPCODE E ADRESS
  while (!(SPSR & (1 << SPIF)));
  SPDR = data;  // O DADO A SER ENVIADO (SEGUNDO ENVIO)
  while (!(SPSR & (1 << SPIF)));
  digitalWrite(CS, 1);  // ELEVANDO O NÍVEL DO CS PARA ENCERRAR A COMUNICAÇÃO
}


// PARA OPERAÇÃO DE LEITURA SÃO USADOS 02 (DOIS) BYTES, ASSIM, DEVE SER FEITO DOIS ENVIOS
// ESCREVENDO O OPCODE + ADRESS (PRIMEIRO ENVIO)

unsigned char read_register(unsigned char opcode, unsigned char adress) {
  digitalWrite(CS, 0);     // ABAIXANDO O NÍVEL DO CS BAIXO PARA INICIAR A COMUNICAÇÃO
  SPDR = opcode | adress;  // CONCATENANDO O OPCODE E ADRESS
  while (!(SPSR & (1 << SPIF)));
  SPDR  = 0x00;  // O SEGUNDO ENVIO É UM DADO É VAZIO
  while (!(SPSR & (1 << SPIF)));
  digitalWrite(CS, 1);  // ELEVANDO O NÍVEL DO CS PARA ENCERRAR A COMUNICAÇÃO
  return SPDR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Configrar o ECON 1 para conseguirmos entrar no Banco 3
// bit 7: 0 = Normal operation
// bit 6: 0 = Normal operations
// bit 5: 0 = DMA hardware is Idle
// bit 4: 0 = DMA hardware copies buffer memory
// bit 3: 0 = The transmit logic is Idle
// bit 2: 0 = All packets received will be ignored
// bit 1-0: 11 = SPI accesses registers in Bank 3
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// OPERAÇÃO PARA LIMPAR O ENDEREÇO DE UM BANCO NO ECON1 - Página 31 [ENC 28J60]
void clear_bank(unsigned char bank) {
  digitalWrite(CS, 0);  // ABAIXANDO O NÍVEL DO CS BAIXO PARA INICIAR A COMUNICAÇÃO

  SPDR = (OPBFC | ECON1);  // LIMPANDO O BITs 1-0, ONDE CONTINHA O ANTIGO ENDEREÇO DO BANCO
  while (!(SPSR & (1 << SPIF)));

  SPDR = (0x03);  // A OPERAÇÃO OR ENTRE 0x02 e 0x01 resulta em 0b00000011
  while (!(SPSR & (1 << SPIF)));

  digitalWrite(CS, 1);  // ELEVANDO O NÍVEL DO CS BAIXO PARA ENCERRAR A COMUNICAÇÃO
}

// OPERAÇÃO PARA SELECIONAR UM BANCO ESPECÍFICO - Página 31 [ENC 28J60]
void set_bank(unsigned char bank) {

  clear_bank(bank);

  digitalWrite(CS, 0);  // ABAIXANDO O NÍVEL DO CS BAIXO PARA INICIAR A COMUNICAÇÃO

  SPDR = (OPBFS | ECON1);
  while (!(SPSR & (1 << SPIF)));

  SPDR = bank;  // ESCREVENDO O ENDEREÇO DO BANCO QUE QUERO ADENTRAR
  while (!(SPSR & (1 << SPIF)));

  digitalWrite(CS, 1);  // ELEVANDO O NÍVEL DO CS BAIXO PARA ENCERRAR A COMUNICAÇÃO
}
void reset_enc() {
  digitalWrite(CS, 0);     // ABAIXANDO O NÍVEL DO CS BAIXO PARA INICIAR A COMUNICAÇÃO
  SPDR = 0xFF;  // CONCATENANDO O OPCODE E ADRESS
  while (!(SPSR & (1 << SPIF)));
 
  digitalWrite(CS, 1);  // ELEVANDO O NÍVEL DO CS PARA ENCERRAR A COMUNICAÇÃO
   delay(1);
}
void initialize_enc(){
  // reset
  reset_enc();
  set_bank(0x02);
  write_register(OPWCR,MACON1,0b00001101);//CONFIGURANDO MACON1 PARA OPERAÇAO FULL DUPLEX
  
  write_register(OPWCR,MACON3,0b11110011);//CONFIGURANDO MACON3 PARA QUE ADICIONE O PADDING DO PROTOCOLO ETHERNET E MAC || 0 b 1 1 1 1 0 0 1 1


  // configfurando tamanho em binario.
  // para um valor de 1518 bytes é necessario usar mais que 8 bits,
  // por isso é utilizado o registrador alto e baixo
  write_register(OPWCR,MAMXFLH,0b00000101);
  write_register(OPWCR,MAMXFLL,0b11101110);
  
}
