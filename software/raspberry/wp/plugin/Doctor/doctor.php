<?php
/**
 * Plugin Name: doctor
 * Plugin URI: https://maurinsoft.com.br
 * Description: Um protótipo de plugin para WordPress.
 * Version: 1.0.0
 * Author: Marcelo Maurin Martins
 * Author URI: https://maurinsoft.com.br
 * License: GPL2
 */

// Evitar acesso direto ao arquivo
if (!defined('ABSPATH')) {
    exit;
}

// Função para criar tabelas no banco de dados
function doctor_plugin_criar_tabelas() {
    global $wpdb;

    // Prefixo do banco de dados
    $prefixo = $wpdb->prefix;

    // SQL para criar as tabelas
    $tabelas = [
        'amostras' => "
            CREATE TABLE IF NOT EXISTS `amostras` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Codbar` varchar(50) NOT NULL,
              `id_cliente` int(11) NOT NULL,
              `status` int(11) NOT NULL DEFAULT 0,
              PRIMARY KEY (`id`)
            );
        ",
        'cargos' => "
            CREATE TABLE IF NOT EXISTS `cargos` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Descricao` varchar(50) NOT NULL,
              `IsAdmin` int(11) NOT NULL DEFAULT 0,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'clientes' => "
            CREATE TABLE IF NOT EXISTS `clientes` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(40) NOT NULL,
              `DOC` varchar(20) NOT NULL,
              `Sexo` char(1) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'comandos' => "
            CREATE TABLE IF NOT EXISTS `comandos` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Descricao` varchar(50) NOT NULL,
              `Status` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'etiquetas' => "
            CREATE TABLE IF NOT EXISTS `etiquetas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `idCliente` int(11) NOT NULL,
              `idExame` int(11) NOT NULL,
              `Status` int(11) NOT NULL DEFAULT 0,
              PRIMARY KEY (`id`)
            );
        ",
        'exames' => "
            CREATE TABLE IF NOT EXISTS `exames` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(40) NOT NULL,
              `id_Receita` int(11) NOT NULL,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'logouve' => "
            CREATE TABLE IF NOT EXISTS `logouve` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `texto` varchar(500) NOT NULL,
              `dtcad` date DEFAULT NULL,
              `idComando` int(11) DEFAULT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'pessoas' => "
            CREATE TABLE IF NOT EXISTS `pessoas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(50) NOT NULL,
              `id_cargo` int(11) NOT NULL,
              `Ativo` int(11) NOT NULL DEFAULT 0,
              `chave` varchar(25) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
        'receitas' => "
            CREATE TABLE IF NOT EXISTS `receitas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Receita` varchar(40) NOT NULL,
              `Arquivo` varchar(255) NOT NULL,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            );
        ",
    ];

    // Executar cada comando SQL
    require_once(ABSPATH . 'wp-admin/includes/upgrade.php');
    foreach ($tabelas as $sql) {
        dbDelta($sql);
    }
}

// Gancho para ativação do plugin
register_activation_hook(__FILE__, 'doctor_plugin_criar_tabelas');

// Função para adicionar um menu ao painel do WordPress
function doctor_plugin_adicionar_menu() {
    add_menu_page(
        'Doctor', // Título da página
        'Doctor Menu', // Nome do menu
        'manage_options', // Capacidade requerida
        'doctor-plugin', // Slug do menu
        'doctor_plugin_config', // Função callback
        'dashicons-admin-tools', // Ícone do menu
        90 // Posição do menu
    );
}
add_action('admin_menu', 'doctor_plugin_adicionar_menu');

// Função para exibir a página de configurações
function doctor_plugin_config() {
    ?>
    <div class="wrap">
        <h1>Configurações do Doctor Plugin</h1>
        <form method="post" action="options.php">
            <?php
            settings_fields('doctor_plugin_configuracoes');
            do_settings_sections('doctor-plugin');
            submit_button();
            ?>
        </form>
    </div>
    <?php
}

// Registrar configurações
function doctor_plugin_registrar_configuracoes() {
    register_setting('doctor_plugin_configuracoes', 'doctor_plugin_opcao_exemplo');

    add_settings_section(
        'doctor_plugin_secao_principal',
        'Configurações Gerais',
        null,
        'doctor-plugin'
    );

    add_settings_field(
        'doctor_plugin_opcao_exemplo',
        'Opção de Exemplo',
        'doctor_plugin_input_exemplo',
        'doctor-plugin',
        'doctor_plugin_secao_principal'
    );
}
add_action('admin_init', 'doctor_plugin_registrar_configuracoes');

// Campo de entrada para a página de configurações
function doctor_plugin_input_exemplo() {
    $valor = get_option('doctor_plugin_opcao_exemplo');
    echo '<input type="text" name="doctor_plugin_opcao_exemplo" value="' . esc_attr($valor) . '" />';
}

// CRUD para a tabela "amostras" com funcionalidade de pesquisa por paciente
function shortcode_crud_amostras() {
    global $wpdb;
    $table_name =    'amostras';
    $pacientes_table =    'clientes'; // Relacionamento com a tabela de clientes

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Codbar' => sanitize_text_field($_POST['Codbar']),
                'id_cliente' => intval($_POST['id_cliente']),
                'status' => intval($_POST['status']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Codbar' => sanitize_text_field($_POST['Codbar']),
                'id_cliente' => intval($_POST['id_cliente']),
                'status' => intval($_POST['status']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Condição para a pesquisa
    $search_query = '';
    if (!empty($_GET['search_paciente'])) {
        $search_paciente = sanitize_text_field($_GET['search_paciente']);
        $search_query = "WHERE p.Nome LIKE '%$search_paciente%'";
    }

    // Fetch data com a condição de pesquisa
    $amostras = $wpdb->get_results(
        "SELECT a.*,
        a.id_cliente id_paciente,
        p.Nome as paciente_nome,
        CASE a.status
            WHEN 1 THEN 'Ativo'
            ELSE 'Inativo'
        END AS status_label
        FROM amostras a
        left JOIN clientes p ON a.id_cliente = p.id
        $search_query",
        ARRAY_A
    );

    // Fetch pacientes para o formulário
    $pacientes = $wpdb->get_results("SELECT id, Nome FROM $pacientes_table", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Amostras</h2>

        <!-- Formulário de pesquisa -->
        <form method="get" class="row g-3 mb-4">
            <div class="col-md-8">
                <label for="search_paciente" class="form-label">Pesquisar por Paciente</label>
                <input type="text" name="search_paciente" class="form-control" value="<?php echo isset($_GET['search_paciente']) ? esc_html($_GET['search_paciente']) : ''; ?>">
            </div>
            <div class="col-md-4 align-self-end">
                <button type="submit" class="btn btn-primary">Pesquisar</button>
            </div>
        </form>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="Codbar" class="form-label">Código de Barras</label>
                <input type="text" name="Codbar" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="id_paciente" class="form-label">Paciente</label>
                <select name="id_paciente" class="form-control" required>
                    <option value="">Selecione um Paciente</option>
                    <?php foreach ($pacientes as $paciente): ?>
                        <option value="<?php echo $paciente['id']; ?>">
                            <?php echo esc_html($paciente['Nome']); ?>
                        </option>
                    <?php endforeach; ?>
                </select>
            </div>
            <div class="col-md-4">
                <label for="status" class="form-label">Status</label>
                <select name="status" class="form-control" required>
                    <option value="1">Ativo</option>
                    <option value="0">Inativo</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>
 
        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Código de Barras</th>
                    <th>id Paciente</th>
                    <th>Paciente</th>
                    <th>Status</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($amostras as $amostra): ?>
                    <tr>
                        <td><?php echo $amostra['id']; ?></td>
                        <td><?php echo esc_html($amostra['Codbar']); ?></td>
                        <td><?php echo esc_html($amostra['id_paciente']); ?></td>
                        <td><?php echo esc_html($amostra['paciente_nome']); ?></td>
                        <td><?php echo esc_html($amostra['status_label']); ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $amostra['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $amostra['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_amostras', 'shortcode_crud_amostras');


// CRUD para a tabela "cargos" com layout aprimorado usando Bootstrap
function shortcode_crud_cargos() {
    global $wpdb;
    $table_name =    'cargos';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Descricao' => sanitize_text_field($_POST['Descricao']),
                'IsAdmin' => intval($_POST['IsAdmin']),
                'Ativo' => intval($_POST['Ativo']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Descricao' => sanitize_text_field($_POST['Descricao']),
                'IsAdmin' => intval($_POST['IsAdmin']),
                'Ativo' => intval($_POST['Ativo']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $cargos = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Cargos</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="Descricao" class="form-label">Descrição</label>
                <input type="text" name="Descricao" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="IsAdmin" class="form-label">É Administrador</label>
                <select name="IsAdmin" class="form-control" required>
                    <option value="1">Sim</option>
                    <option value="0">Não</option>
                </select>
            </div>
            <div class="col-md-4">
                <label for="Ativo" class="form-label">Ativo</label>
                <select name="Ativo" class="form-control" required>
                    <option value="1">Sim</option>
                    <option value="0">Não</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Descrição</th>
                    <th>É Administrador</th>
                    <th>Ativo</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($cargos as $cargo): ?>
                    <tr>
                        <td><?php echo $cargo['id']; ?></td>
                        <td><?php echo esc_html($cargo['Descricao']); ?></td>
                        <td><?php echo $cargo['IsAdmin'] ? 'Sim' : 'Não'; ?></td>
                        <td><?php echo $cargo['Ativo'] ? 'Sim' : 'Não'; ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $cargo['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $cargo['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_cargos', 'shortcode_crud_cargos');

// CRUD para a tabela "clientes" com layout aprimorado usando Bootstrap
function shortcode_crud_clientes() {
    global $wpdb;
    $table_name =    'clientes';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'DOC' => sanitize_text_field($_POST['DOC']),
                'Sexo' => sanitize_text_field($_POST['Sexo']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'DOC' => sanitize_text_field($_POST['DOC']),
                'Sexo' => sanitize_text_field($_POST['Sexo']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $clientes = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Clientes</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="Nome" class="form-label">Nome</label>
                <input type="text" name="Nome" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="DOC" class="form-label">Documento</label>
                <input type="text" name="DOC" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="Sexo" class="form-label">Sexo</label>
                <select name="Sexo" class="form-control" required>
                    <option value="M">Masculino</option>
                    <option value="F">Feminino</option>
                    <option value="O">Outro</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Nome</th>
                    <th>Documento</th>
                    <th>Sexo</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($clientes as $cliente): ?>
                    <tr>
                        <td><?php echo $cliente['id']; ?></td>
                        <td><?php echo esc_html($cliente['Nome']); ?></td>
                        <td><?php echo esc_html($cliente['DOC']); ?></td>
                        <td><?php echo esc_html($cliente['Sexo']); ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $cliente['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $cliente['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_clientes', 'shortcode_crud_clientes');


// CRUD para a tabela "comandos" com layout aprimorado usando Bootstrap
function shortcode_crud_comandos() {
    global $wpdb;
    $table_name =    'comandos';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Descricao' => sanitize_text_field($_POST['Descricao']),
                'Status' => intval($_POST['Status']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Descricao' => sanitize_text_field($_POST['Descricao']),
                'Status' => intval($_POST['Status']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $comandos = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Comandos</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-6">
                <label for="Descricao" class="form-label">Descrição</label>
                <input type="text" name="Descricao" class="form-control" required>
            </div>
            <div class="col-md-6">
                <label for="Status" class="form-label">Status</label>
                <select name="Status" class="form-control" required>
                    <option value="1">Ativo</option>
                    <option value="0">Inativo</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Descrição</th>
                    <th>Status</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($comandos as $comando): ?>
                    <tr>
                        <td><?php echo $comando['id']; ?></td>
                        <td><?php echo esc_html($comando['Descricao']); ?></td>
                        <td><?php echo $comando['Status'] ? 'Ativo' : 'Inativo'; ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $comando['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $comando['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_comandos', 'shortcode_crud_comandos');

// CRUD para a tabela "etiquetas" com layout aprimorado usando Bootstrap
function shortcode_crud_etiquetas() {
    global $wpdb;
    $table_name =    'etiquetas';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'idCliente' => intval($_POST['idCliente']),
                'idExame' => intval($_POST['idExame']),
                'Status' => intval($_POST['Status']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'idCliente' => intval($_POST['idCliente']),
                'idExame' => intval($_POST['idExame']),
                'Status' => intval($_POST['Status']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $etiquetas = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Etiquetas</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="idCliente" class="form-label">ID Cliente</label>
                <input type="number" name="idCliente" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="idExame" class="form-label">ID Exame</label>
                <input type="number" name="idExame" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="Status" class="form-label">Status</label>
                <select name="Status" class="form-control" required>
                    <option value="1">Ativo</option>
                    <option value="0">Inativo</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>ID Cliente</th>
                    <th>ID Exame</th>
                    <th>Status</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($etiquetas as $etiqueta): ?>
                    <tr>
                        <td><?php echo $etiqueta['id']; ?></td>
                        <td><?php echo esc_html($etiqueta['idCliente']); ?></td>
                        <td><?php echo esc_html($etiqueta['idExame']); ?></td>
                        <td><?php echo $etiqueta['Status'] ? 'Ativo' : 'Inativo'; ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $etiqueta['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $etiqueta['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_etiquetas', 'shortcode_crud_etiquetas');

// CRUD para a tabela "exames" com layout aprimorado usando Bootstrap
function shortcode_crud_exames() {
    global $wpdb;
    $table_name =    'exames';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'id_Receita' => intval($_POST['id_Receita']),
                'Ativo' => intval($_POST['Ativo']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'id_Receita' => intval($_POST['id_Receita']),
                'Ativo' => intval($_POST['Ativo']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $exames = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Exames</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="Nome" class="form-label">Nome</label>
                <input type="text" name="Nome" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="id_Receita" class="form-label">ID Receita</label>
                <input type="number" name="id_Receita" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="Ativo" class="form-label">Ativo</label>
                <select name="Ativo" class="form-control" required>
                    <option value="1">Sim</option>
                    <option value="0">Não</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Nome</th>
                    <th>ID Receita</th>
                    <th>Ativo</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($exames as $exame): ?>
                    <tr>
                        <td><?php echo $exame['id']; ?></td>
                        <td><?php echo esc_html($exame['Nome']); ?></td>
                        <td><?php echo esc_html($exame['id_Receita']); ?></td>
                        <td><?php echo $exame['Ativo'] ? 'Sim' : 'Não'; ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $exame['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $exame['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_exames', 'shortcode_crud_exames');

// CRUD para a tabela "logouve" com layout aprimorado usando Bootstrap
function shortcode_crud_logouve() {
    global $wpdb;
    $table_name =    'logouve';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'texto' => sanitize_text_field($_POST['texto']),
                'dtcad' => sanitize_text_field($_POST['dtcad']),
                'idComando' => intval($_POST['idComando']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'texto' => sanitize_text_field($_POST['texto']),
                'dtcad' => sanitize_text_field($_POST['dtcad']),
                'idComando' => intval($_POST['idComando']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $logouve = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Logouve</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-6">
                <label for="texto" class="form-label">Texto</label>
                <input type="text" name="texto" class="form-control" required>
            </div>
            <div class="col-md-3">
                <label for="dtcad" class="form-label">Data</label>
                <input type="date" name="dtcad" class="form-control" required>
            </div>
            <div class="col-md-3">
                <label for="idComando" class="form-label">ID Comando</label>
                <input type="number" name="idComando" class="form-control" required>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Texto</th>
                    <th>Data</th>
                    <th>ID Comando</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($logouve as $log): ?>
                    <tr>
                        <td><?php echo $log['id']; ?></td>
                        <td><?php echo esc_html($log['texto']); ?></td>
                        <td><?php echo esc_html($log['dtcad']); ?></td>
                        <td><?php echo esc_html($log['idComando']); ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $log['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $log['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_logouve', 'shortcode_crud_logouve');

// CRUD para a tabela "pessoas" com layout aprimorado usando Bootstrap
function shortcode_crud_pessoas() {
    global $wpdb;
    $table_name =    'pessoas';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'id_cargo' => intval($_POST['id_cargo']),
                'Ativo' => intval($_POST['Ativo']),
                'chave' => sanitize_text_field($_POST['chave']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Nome' => sanitize_text_field($_POST['Nome']),
                'id_cargo' => intval($_POST['id_cargo']),
                'Ativo' => intval($_POST['Ativo']),
                'chave' => sanitize_text_field($_POST['chave']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $pessoas = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Pessoas</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-3">
                <label for="Nome" class="form-label">Nome</label>
                <input type="text" name="Nome" class="form-control" required>
            </div>
            <div class="col-md-3">
                <label for="id_cargo" class="form-label">ID Cargo</label>
                <input type="number" name="id_cargo" class="form-control" required>
            </div>
            <div class="col-md-3">
                <label for="Ativo" class="form-label">Ativo</label>
                <select name="Ativo" class="form-control" required>
                    <option value="1">Sim</option>
                    <option value="0">Não</option>
                </select>
            </div>
            <div class="col-md-3">
                <label for="chave" class="form-label">Chave</label>
                <input type="text" name="chave" class="form-control" required>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-hover">
                <tr>
                    <th>ID</th>
                    <th>Nome</th>
                    <th>ID Cargo</th>
                    <th>Ativo</th>
                    <th>Chave</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($pessoas as $pessoa): ?>
                    <tr>
                        <td><?php echo $pessoa['id']; ?></td>
                        <td><?php echo esc_html($pessoa['Nome']); ?></td>
                        <td><?php echo esc_html($pessoa['id_cargo']); ?></td>
                        <td><?php echo $pessoa['Ativo'] ? 'Sim' : 'Não'; ?></td>
                        <td><?php echo esc_html($pessoa['chave']); ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $pessoa['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $pessoa['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_pessoas', 'shortcode_crud_pessoas');

// CRUD para a tabela "receitas" com layout aprimorado usando Bootstrap
function shortcode_crud_receitas() {
    global $wpdb;
    $table_name =    'receitas';

    // Handle form submissions
    if ($_SERVER['REQUEST_METHOD'] === 'POST') {
        if (isset($_POST['insert'])) {
            $wpdb->insert($table_name, [
                'Receita' => sanitize_text_field($_POST['Receita']),
                'Arquivo' => sanitize_text_field($_POST['Arquivo']),
                'Ativo' => intval($_POST['Ativo']),
            ]);
        } elseif (isset($_POST['update'])) {
            $wpdb->update($table_name, [
                'Receita' => sanitize_text_field($_POST['Receita']),
                'Arquivo' => sanitize_text_field($_POST['Arquivo']),
                'Ativo' => intval($_POST['Ativo']),
            ], ['id' => intval($_POST['id'])]);
        } elseif (isset($_POST['delete'])) {
            $wpdb->delete($table_name, ['id' => intval($_POST['id'])]);
        }
    }

    // Fetch data
    $receitas = $wpdb->get_results("SELECT * FROM $table_name", ARRAY_A);

    // Render the form and data table
    ob_start();
    ?>
    <div class="container mt-4">
        <h2 class="mb-4">Gerenciar Receitas</h2>

        <!-- Formulário de inserção -->
        <form method="post" class="row g-3 mb-4">
            <div class="col-md-4">
                <label for="Receita" class="form-label">Receita</label>
                <input type="text" name="Receita" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="Arquivo" class="form-label">Arquivo</label>
                <input type="text" name="Arquivo" class="form-control" required>
            </div>
            <div class="col-md-4">
                <label for="Ativo" class="form-label">Ativo</label>
                <select name="Ativo" class="form-control" required>
                    <option value="1">Sim</option>
                    <option value="0">Não</option>
                </select>
            </div>
            <div class="col-12">
                <button type="submit" name="insert" class="btn btn-primary">Inserir</button>
            </div>
        </form>

        <!-- Tabela de dados -->
        <table class="table table-hover table-light">
            <thead class="table-light">
                <tr>
                    <th>ID</th>
                    <th>Receita</th>
                    <th>Arquivo</th>
                    <th>Ativo</th>
                    <th>Ações</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($receitas as $receita): ?>
                    <tr>
                        <td><?php echo $receita['id']; ?></td>
                        <td><?php echo esc_html($receita['Receita']); ?></td>
                        <td><?php echo esc_html($receita['Arquivo']); ?></td>
                        <td><?php echo $receita['Ativo'] ? 'Sim' : 'Não'; ?></td>
                        <td>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $receita['id']; ?>">
                                <button type="submit" name="edit" class="btn btn-warning btn-sm">Editar</button>
                            </form>
                            <form method="post" class="d-inline">
                                <input type="hidden" name="id" value="<?php echo $receita['id']; ?>">
                                <button type="submit" name="delete" class="btn btn-danger btn-sm">Excluir</button>
                            </form>
                        </td>
                    </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    </div>
    <?php
    return ob_get_clean();
}
add_shortcode('crud_receitas', 'shortcode_crud_receitas');
