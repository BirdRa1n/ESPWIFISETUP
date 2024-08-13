## Introdução

Este projeto utiliza um módulo ESP8266 para conectar-se a uma rede Wi-Fi e permitir a configuração via um servidor web local. O dispositivo pode ser configurado para se conectar a uma rede Wi-Fi específica, e, caso a conexão falhe, ele cria um ponto de acesso (AP) para permitir a configuração da rede Wi-Fi.

## Estrutura do Código

### Bibliotecas Utilizadas

- **Wire.h**: Biblioteca para comunicação I2C.
- **ESP8266WiFi.h**: Biblioteca para gerenciamento de conexões Wi-Fi com ESP8266.
- **ESP8266HTTPClient.h**: Biblioteca para realizar requisições HTTP.
- **WiFiClient.h**: Biblioteca para gerenciamento de clientes Wi-Fi.
- **ArduinoJson.h**: Biblioteca para manipulação de JSON.
- **ESP8266WebServer.h**: Biblioteca para criar um servidor web.
- **EEPROM.h**: Biblioteca para armazenar e recuperar dados na EEPROM.

### Constantes e Definições

- **EEPROM_SIZE**: Tamanho da EEPROM.
- **SSID_START_ADDR**: Endereço inicial na EEPROM para armazenar o SSID.
- **PASS_START_ADDR**: Endereço inicial na EEPROM para armazenar a senha.
- **SSID_LENGTH**: Comprimento máximo do SSID.
- **PASS_LENGTH**: Comprimento máximo da senha.
- **BUTTON_RESET_PIN**: Pino do botão de reset.
- **MAX_WIFI_WAIT_TIME**: Tempo máximo para tentar conectar-se à rede Wi-Fi.

### Funções

#### `setup()`

Inicializa o monitor serial, configura a EEPROM e verifica se o botão de reset está pressionado. Se o botão estiver pressionado, inicia um ponto de acesso e o servidor web. Caso contrário, tenta conectar-se à rede Wi-Fi usando as credenciais armazenadas na EEPROM. Se a conexão falhar, inicia um ponto de acesso.

#### `loop()`

Lida com as requisições do servidor web. Se o ESP8266 estiver conectado à rede Wi-Fi, imprime o IP local e aguarda. Caso contrário, reinicializa o ESP8266.

#### `connectToWiFi(const String& ssid, const String& pass)`

Tenta conectar-se à rede Wi-Fi com o SSID e a senha fornecidos. Retorna `true` se a conexão for bem-sucedida e `false` caso contrário.

#### `startAccessPoint()`

Configura o ESP8266 para funcionar como um ponto de acesso com o SSID e senha definidos. Imprime o IP do ponto de acesso.

#### `setupWebServer()`

Configura as rotas do servidor web para lidar com as requisições.

#### `handleNetworkInfo()`

Retorna informações sobre a rede Wi-Fi atualmente conectada, como SSID e intensidade do sinal (RSSI).

#### `handleNetworkList()`

Retorna uma lista de redes Wi-Fi disponíveis com informações sobre cada uma, incluindo SSID, intensidade do sinal (RSSI) e tipo de criptografia.

#### `handleSettings()`

Recebe e armazena as novas credenciais Wi-Fi enviadas via uma requisição POST. Se as credenciais forem válidas, salva-as na EEPROM e reinicializa o ESP8266. Caso contrário, envia uma mensagem de erro.

#### `handleRoot()`

Resposta padrão para a raiz do servidor web. Retorna um JSON com uma mensagem de status "ok".

#### `readStringFromEEPROM(int startAddress, int length)`

Lê uma string da EEPROM a partir do endereço e comprimento fornecidos.

#### `writeStringToEEPROM(int startAddress, const String& data)`

Escreve uma string na EEPROM a partir do endereço fornecido.

#### `restartESP()`

Reinicializa o ESP8266.

#### `serializeJsonString(const DynamicJsonDocument& doc)`

Serializa um objeto JSON para uma string.

## Configuração do Hardware

- **Pino do Botão de Reset (D8)**: Este pino é usado para iniciar o ponto de acesso e o servidor web quando pressionado. Certifique-se de que o botão de reset esteja corretamente conectado ao pino D8 do ESP8266.

## Uso

1. **Inicialização**: Quando o dispositivo é inicializado, ele tenta conectar-se à rede Wi-Fi usando as credenciais armazenadas na EEPROM. Se a conexão falhar ou o botão de reset estiver pressionado, o dispositivo atuará como um ponto de acesso.

2. **Configuração via Web**: Conecte-se à rede Wi-Fi criada pelo ESP8266 (SSID: "ESP8266"). Acesse o servidor web em http://192.168.4.1 para configurar as credenciais da rede Wi-Fi.

3. **Rotas Disponíveis**:
   - **`/networkinfo`**: Retorna informações sobre a rede Wi-Fi conectada.
   - **`/networks`**: Retorna uma lista das redes Wi-Fi disponíveis.
   - **`/setting`**: Recebe as credenciais da rede Wi-Fi e as armazena na EEPROM.
   - **`/`**: Retorna um JSON com a mensagem "ok".

## Considerações Finais

Este código foi projetado para ser um ponto de partida para projetos que requerem configuração de rede Wi-Fi via web. Pode ser adaptado e expandido para atender às necessidades específicas do projeto.
