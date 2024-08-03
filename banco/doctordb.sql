-- phpMyAdmin SQL Dump
-- version 5.1.1deb5ubuntu1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Tempo de geração: 29/07/2024 às 13:04
-- Versão do servidor: 8.0.37-0ubuntu0.22.04.3
-- Versão do PHP: 8.1.2-1ubuntu2.18

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Banco de dados: doctordb
--

-- --------------------------------------------------------

--
-- Estrutura para tabela amostras
--

CREATE TABLE amostras (
  id int NOT NULL,
  Codbar varchar(50) NOT NULL,
  id_cliente int NOT NULL,
  Status int NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela amostras
--

INSERT INTO amostras (id, Codbar, id_cliente, Status) VALUES
(32, '32', 1, 0),
(33, '33', 4, 0),
(34, '34', 3, 0),
(35, '35', 3, 0),
(36, '36', 3, 0),
(37, '37', 3, 0),
(38, '38', 3, 0),
(39, '39', 3, 0),
(40, '40', 3, 0),
(41, '41', 3, 0);

-- --------------------------------------------------------

--
-- Estrutura para tabela cargos
--

CREATE TABLE cargos (
  id int NOT NULL,
  Descricao varchar(50) NOT NULL,
  IsAdmin int NOT NULL DEFAULT '0',
  Ativo int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela cargos
--

INSERT INTO cargos (id, Descricao, IsAdmin, Ativo) VALUES
(1, 'Administrador', 1, 1);

-- --------------------------------------------------------

--
-- Estrutura para tabela clientes
--

CREATE TABLE clientes (
  id int NOT NULL,
  Nome varchar(40) NOT NULL,
  DOC varchar(20) NOT NULL,
  Sexo char(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela clientes
--

INSERT INTO clientes (id, Nome, DOC, Sexo) VALUES
(1, 'Cleusa Catarina Maurin Martins', '12222222', 'F'),
(2, 'Marcella Maurin Martins', '22222', 'F'),
(3, 'Camille Maurin Martins', '145566666', 'F'),
(4, 'Daniela Machado', '53331122', 'F');

-- --------------------------------------------------------

--
-- Estrutura para tabela comandos
--

CREATE TABLE comandos (
  id int NOT NULL,
  Descricao varchar(50) NOT NULL,
  Status int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela comandos
--

INSERT INTO comandos (id, Descricao, Status) VALUES
(1, 'Bom dia', 1),
(2, 'Status equipamento', 1),
(3, 'Sair do Sistema', 1),
(4, 'Etiquetagem', 1);

-- --------------------------------------------------------

--
-- Estrutura para tabela etiquetas
--

CREATE TABLE etiquetas (
  id int NOT NULL,
  idCliente int NOT NULL,
  idExame int NOT NULL,
  Status int NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

-- --------------------------------------------------------

--
-- Estrutura para tabela exames
--

CREATE TABLE exames (
  id int NOT NULL,
  Nome varchar(40) NOT NULL,
  id_Receita int NOT NULL,
  Ativo int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela exames
--

INSERT INTO exames (id, Nome, id_Receita, Ativo) VALUES
(1, 'Leitura PH', 1, 1);

-- --------------------------------------------------------

--
-- Estrutura para tabela logouve
--

CREATE TABLE logouve (
  id int NOT NULL,
  texto varchar(500) NOT NULL,
  dtcad date DEFAULT NULL,
  idComando int DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela logouve
--

INSERT INTO logouve (id, texto, dtcad, idComando) VALUES
(5, 'Olá Doctor', '2024-07-29', 1),
(6, 'Doctor Me dê o status do equipamentoDoctor Me dê o status do equipamento', '2024-07-29', 2),
(7, 'Doctor me deu statusDoctor me deu status', '2024-07-29', 2),
(8, 'por favor do Estado Doctor', '2024-07-29', 2),
(9, 'Doctor Qual a condição do equipamento', '2024-07-29', 2),
(10, 'Doctor me dê um status RepórterDoctor me dê um status Repórter', '2024-07-29', 2),
(11, 'Doctor me dê um status', '2024-07-29', 2),
(12, 'Doctor por favor me dá o statusDoctor por favor me dá o status', '2024-07-29', 2),
(13, 'Doctor Qual a condição do projeto', '2024-07-29', 2),
(14, 'status repórter Doctor', '2024-07-29', 2),
(15, 'status do equipamento por favor Doctorstatus do equipamento por favor Doctor', '2024-07-29', 2),
(16, 'Bom dia DoctorBom dia Doctor', '2024-07-29', 1),
(17, 'Olá Doctor tudo bemOlá Doctor tudo bem', '2024-07-29', 1),
(18, 'Boa tarde Doctor', '2024-07-29', 1),
(19, 'Boa noite Doctor', '2024-07-29', 1),
(20, 'como vai você Doctorcomo vai você Doctor', '2024-07-29', NULL),
(21, 'como vai você Doctor', '2024-07-29', NULL),
(22, 'identificar etiqueta Doctoridentificar etiqueta Doctor', '2024-07-29', NULL),
(23, 'mostrar análise dos faltosos Doctormostrar análise dos faltosos Doctor', '2024-07-29', NULL),
(24, 'mostrar análise pendente Doctormostrar análise pendente Doctor', '2024-07-29', NULL),
(25, 'Olá Doctor', '2024-07-29', 1),
(26, 'aqui é o Doctoraqui é o Doctor', '2024-07-29', 1),
(27, 'iniciando o CD novo do Doctor aguarde', '2024-07-29', NULL),
(28, 'Bom dia DoctorBom dia Doctor', '2024-07-29', NULL),
(29, 'Bom dia Doctor', '2024-07-29', NULL),
(30, 'Doctor status RepórterDoctor status Repórter', '2024-07-29', NULL),
(31, 'Bom dia DoctorBom dia Doctor', '2024-07-29', NULL),
(32, 'Bom dia DoctorBom dia Doctor', '2024-07-29', NULL),
(33, 'Doctor como vai você', '2024-07-29', NULL),
(34, 'Boa noite DoctorBoa noite Doctor', '2024-07-29', NULL),
(35, 'Olá DoctorOlá Doctor', '2024-07-29', 1),
(36, 'Bom dia DoctorBom dia Doctor', '2024-07-29', 1),
(37, 'Olá Doctor', '2024-07-29', 1),
(38, 'Boa noite DoctorBoa noite Doctor', '2024-07-29', NULL),
(39, 'Boa noite Doctor', '2024-07-29', 1),
(40, 'Bom dia DoctorBom dia Doctor', '2024-07-29', 1),
(41, 'Boa noite DoctorBoa noite Doctor', '2024-07-29', 1),
(42, 'como vai você Doctor', '2024-07-29', 1),
(43, 'Adeus Doctor', '2024-07-29', 3),
(44, 'bye bye Doctorbye bye Doctor', '2024-07-29', 3),
(45, 'Bom dia DoctorBom dia Doctor', '2024-07-29', NULL),
(46, 'Olá DoctorOlá Doctor', '2024-07-29', NULL),
(47, 'Doctor status RepórterDoctor status Repórter', '2024-07-29', NULL),
(48, 'Bom dia Doctor', '2024-07-29', 1),
(49, 'Bom dia Doctor', '2024-07-29', 1),
(50, 'Boa noite DoctorBoa noite Doctor', '2024-07-29', 1),
(51, 'Doctor me dê um status RepórterDoctor me dê um status Repórter', '2024-07-29', 2),
(52, 'me passa status repórter por favor Doctorme passa status repórter por favor Doctor', '2024-07-29', 2),
(53, 'Doctor status Repórter por favor', '2024-07-29', 2),
(54, 'Boa noite Doctor', '2024-07-29', 1),
(55, 'Boa noite DoctorBoa noite Doctor', '2024-07-29', 1),
(56, 'Boa tarde DoctorBoa tarde Doctor', '2024-07-29', 1),
(57, 'Doctor qual status repórter por favorDoctor qual status repórter por favor', '2024-07-29', 2),
(58, 'Boa noite Doctor', '2024-07-29', 1),
(59, 'Bom dia DoctorBom dia Doctor', '2024-07-29', 1),
(60, 'Doctor me dê um status RepórterDoctor me dê um status Repórter', '2024-07-29', 2),
(61, 'sair do programa Doctor', '2024-07-29', 3),
(62, 'bye bye Doctor', '2024-07-29', 3),
(63, 'Doctor', '2024-07-29', 1),
(64, 'Doctor ir no programa de etiquetagemDoctor ir no programa de etiquetagem', '2024-07-29', 4),
(65, 'Doctor ir na tela de etiquetagemDoctor ir na tela de etiquetagem', '2024-07-29', 4),
(66, 'gerar etiqueta Doctor', '2024-07-29', 4),
(67, 'Jerry a etiqueta DoctorJerry a etiqueta Doctor', '2024-07-29', 4),
(68, 'Doctor etiqueta por favor', '2024-07-29', 4),
(69, 'Doctor ir na tela de etiquetaDoctor ir na tela de etiqueta', '2024-07-29', 4),
(70, 'Doctor entrar na tela de etiqueta por favorDoctor entrar na tela de etiqueta por favor', '2024-07-29', 4),
(71, 'Doctor Por favor entre na tela de etiquetaDoctor Por favor entre na tela de etiqueta', '2024-07-29', 4),
(72, 'Doctor etiquetas geradas automaticamenteDoctor etiquetas geradas automaticamente', '2024-07-29', 4),
(73, 'DoctorDoctor', '2024-07-29', 1),
(74, 'Doctor ir no cadastro de Nova etiqueta', '2024-07-29', 4),
(75, 'Doctor cadastrar nova etiqueta', '2024-07-29', 4),
(76, 'Doctor cadastrar nova amostraDoctor cadastrar nova amostra', '2024-07-29', 4),
(77, 'Doctor cadastrar nova amostra com etiquetaDoctor cadastrar nova amostra com etiqueta', '2024-07-29', 4),
(78, 'DoctorDoctor', '2024-07-29', 1),
(79, 'Doctor Gere uma nova etiquetaDoctor Gere uma nova etiqueta', '2024-07-29', 4),
(80, 'Bom dia Doctor', '2024-07-29', NULL),
(81, 'Bom dia Doctor', '2024-07-29', NULL),
(82, 'Doctor Me dê o status do equipamento', '2024-07-29', NULL),
(83, 'Doctor ir no cadastro de etiqueta', '2024-07-29', 4),
(84, 'Doctor Bom diaDoctor Bom dia', '2024-07-29', NULL),
(85, 'Doctor Bom dia', '2024-07-29', NULL),
(86, 'Doctor ir na tela de cadastro de etiqueta', '2024-07-29', 4),
(87, 'Doctor status Repórter', '2024-07-29', 2),
(88, 'Doctor Bom dia', '2024-07-29', NULL),
(89, 'pessoal eu vou mostrar aqui o Doctor que é o Projeto TCC', '2024-07-29', NULL),
(90, 'Doctor Bom dia', '2024-07-29', 1),
(91, 'eu posso falar várias coisas diferentes das funções e interagir e ele consegue rodar vocês estão vendo aqui ó na verdade é o Doctor tá vendoeu posso falar várias coisas diferentes das funções e interagir e ele consegue rodar vocês estão vendo aqui ó na verdade é o Doctor tá vendo', '2024-07-29', NULL),
(92, 'Doctor sair da aplicaçãoDoctor sair da aplicação', '2024-07-29', 3),
(93, 'Doctor', '2024-07-29', 1),
(94, 'sair Doctor Fechar programasair Doctor Fechar programa', '2024-07-29', 3),
(95, 'tem os textos que o Doctor falou desde a hora que na verdade quem falou fui eu né não é o Doctortem os textos que o Doctor falou desde a hora que na verdade quem falou fui eu né não é o Doctor', '2024-07-29', NULL),
(96, 'E aí no Doctor bloquear que o acesso e vou repetir tudo de novoE aí no Doctor bloquear que o acesso e vou repetir tudo de novo', '2024-07-29', NULL),
(97, 'Doctor Bom diaDoctor Bom dia', '2024-07-29', NULL),
(98, 'Doctor status Repórter', '2024-07-29', NULL),
(99, 'Doctor ir na tela de etiquetagemDoctor ir na tela de etiquetagem', '2024-07-29', NULL),
(100, 'pessoal Olha esse aqui é o Doctor meu TCC sistema biomédico', '2024-07-29', NULL),
(101, 'Doctor Bom dia', '2024-07-29', NULL),
(102, 'Doctor me dê um status Repórter', '2024-07-29', NULL),
(103, 'Doctor Me dê o status do equipamentoDoctor Me dê o status do equipamento', '2024-07-29', NULL),
(104, 'eu posso sair da aplicação eu posso ir para uma tela específica Então vamos lá Doctor ir na tela de cadastro de etiquetaeu posso sair da aplicação eu posso ir para uma tela específica Então vamos lá Doctor ir na tela de cadastro de etiqueta', '2024-07-29', NULL);

-- --------------------------------------------------------

--
-- Estrutura para tabela pessoas
--

CREATE TABLE pessoas (
  id int NOT NULL,
  Nome varchar(50) NOT NULL,
  id_cargo int NOT NULL,
  Ativo int NOT NULL DEFAULT '0',
  chave varchar(25) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Despejando dados para a tabela pessoas
--

INSERT INTO pessoas (id, Nome, id_cargo, Ativo, chave) VALUES
(1, 'marcelo', 1, 1, '0005694722'),
(3, 'admin', 1, 0, '0011329555');

-- --------------------------------------------------------

--
-- Estrutura para tabela receitas
--

CREATE TABLE receitas (
  id int NOT NULL,
  Receita varchar(40) NOT NULL,
  Arquivo varchar(255) NOT NULL,
  Ativo int NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;

--
-- Índices para tabelas despejadas
--

--
-- Índices de tabela amostras
--
ALTER TABLE amostras
  ADD PRIMARY KEY (id),
  ADD KEY ind_cliente (id_cliente);

--
-- Índices de tabela cargos
--
ALTER TABLE cargos
  ADD PRIMARY KEY (id);

--
-- Índices de tabela clientes
--
ALTER TABLE clientes
  ADD PRIMARY KEY (id);

--
-- Índices de tabela comandos
--
ALTER TABLE comandos
  ADD PRIMARY KEY (id);

--
-- Índices de tabela etiquetas
--
ALTER TABLE etiquetas
  ADD PRIMARY KEY (id),
  ADD KEY ind_Cliente (idCliente),
  ADD KEY ind_idexame (idExame);

--
-- Índices de tabela exames
--
ALTER TABLE exames
  ADD PRIMARY KEY (id),
  ADD KEY ind_receita (id_Receita);

--
-- Índices de tabela logouve
--
ALTER TABLE logouve
  ADD PRIMARY KEY (id);

--
-- Índices de tabela pessoas
--
ALTER TABLE pessoas
  ADD PRIMARY KEY (id),
  ADD KEY ind_cargo (id_cargo);

--
-- Índices de tabela receitas
--
ALTER TABLE receitas
  ADD PRIMARY KEY (id);

--
-- AUTO_INCREMENT para tabelas despejadas
--

--
-- AUTO_INCREMENT de tabela amostras
--
ALTER TABLE amostras
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=42;

--
-- AUTO_INCREMENT de tabela cargos
--
ALTER TABLE cargos
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT de tabela clientes
--
ALTER TABLE clientes
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de tabela comandos
--
ALTER TABLE comandos
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de tabela etiquetas
--
ALTER TABLE etiquetas
  MODIFY id int NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT de tabela exames
--
ALTER TABLE exames
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;

--
-- AUTO_INCREMENT de tabela logouve
--
ALTER TABLE logouve
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=105;

--
-- AUTO_INCREMENT de tabela pessoas
--
ALTER TABLE pessoas
  MODIFY id int NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;

--
-- AUTO_INCREMENT de tabela receitas
--
ALTER TABLE receitas
  MODIFY id int NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
