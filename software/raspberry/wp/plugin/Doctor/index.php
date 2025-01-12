<?php
/**
 * Plugin Name: Doctor
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
            CREATE TABLE IF NOT EXISTS `{$prefixo}amostras` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Codbar` varchar(50) NOT NULL,
              `id_cliente` int(11) NOT NULL,
              `status` int(11) NOT NULL DEFAULT 0,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'cargos' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}cargos` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Descricao` varchar(50) NOT NULL,
              `IsAdmin` int(11) NOT NULL DEFAULT 0,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'clientes' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}clientes` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(40) NOT NULL,
              `DOC` varchar(20) NOT NULL,
              `Sexo` char(1) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'comandos' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}comandos` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Descricao` varchar(50) NOT NULL,
              `Status` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'etiquetas' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}etiquetas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `idCliente` int(11) NOT NULL,
              `idExame` int(11) NOT NULL,
              `Status` int(11) NOT NULL DEFAULT 0,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'exames' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}exames` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(40) NOT NULL,
              `id_Receita` int(11) NOT NULL,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'logouve' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}logouve` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `texto` varchar(500) NOT NULL,
              `dtcad` date DEFAULT NULL,
              `idComando` int(11) DEFAULT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'pessoas' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}pessoas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Nome` varchar(50) NOT NULL,
              `id_cargo` int(11) NOT NULL,
              `Ativo` int(11) NOT NULL DEFAULT 0,
              `chave` varchar(25) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
        ",
        'receitas' => "
            CREATE TABLE IF NOT EXISTS `{$prefixo}receitas` (
              `id` int(11) NOT NULL AUTO_INCREMENT,
              `Receita` varchar(40) NOT NULL,
              `Arquivo` varchar(255) NOT NULL,
              `Ativo` int(11) NOT NULL,
              PRIMARY KEY (`id`)
            ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;
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

// Shortcodes para CRUD de cada tabela
function shortcode_crud_amostras() {
    return '<div>CRUD para a tabela amostras em construção...</div>';
}
add_shortcode('crud_amostras', 'shortcode_crud_amostras');

function shortcode_crud_cargos() {
    return '<div>CRUD para a tabela cargos em construção...</div>';
}
add_shortcode('crud_cargos', 'shortcode_crud_cargos');

function shortcode_crud_clientes() {
    return '<div>CRUD para a tabela clientes em construção...</div>';
}
add_shortcode('crud_clientes', 'shortcode_crud_clientes');

function shortcode_crud_comandos() {
    return '<div>CRUD para a tabela comandos em construção...</div>';
}
add_shortcode('crud_comandos', 'shortcode_crud_comandos');

function shortcode_crud_etiquetas() {
    return '<div>CRUD para a tabela etiquetas em construção...</div>';
}
add_shortcode('crud_etiquetas', 'shortcode_crud_etiquetas');

function shortcode_crud_exames() {
    return '<div>CRUD para a tabela exames em construção...</div>';
}
add_shortcode('crud_exames', 'shortcode_crud_exames');

function shortcode_crud_logouve() {
    return '<div>CRUD para a tabela logouve em construção...</div>';
}
add_shortcode('crud_logouve', 'shortcode_crud_logouve');

function shortcode_crud_pessoas() {
    return '<div>CRUD para a tabela pessoas em construção...</div>';
}
add_shortcode('crud_pessoas', 'shortcode_crud_pessoas');

function shortcode_crud_receitas() {
    return '<div>CRUD para a tabela receitas em construção...</div>';
}
add_shortcode('crud_receitas', 'shortcode_crud_receitas');
