#!/usr/bin/python3

import mysql.connector
import openai

# ------------------------------------------------------------------------------
# Configuração da API da OpenAI (use a chave fornecida)
openai.api_key = "Digite sua chave aqui"
# ------------------------------------------------------------------------------

def get_db_connection():
    """
    Retorna uma conexão com o banco de dados MySQL.
    Ajuste host, user, password e database conforme suas credenciais.
    """
    return mysql.connector.connect(
        host="localhost",
        user="mmm",
        password="226468",
        database="doctordb"
    )

def classify_text_with_chatgpt(texto, comandos):
    """
    Pergunta ao ChatGPT qual comando da lista é mais relevante para o texto.
    Retorna:
        - O id do comando escolhido (int), se for algum comando.
        - None, se "Nenhuma delas" for escolhida.
    """

    # Monta uma string com as opções de comando
    # Ex: "1) Descrição do comando 1\n2) Descrição do comando 2\n...\n0) Nenhuma delas"
    comandos_opcoes = ""
    for cmd in comandos:
        comandos_opcoes += f"{cmd['id']}) {cmd['Descricao']}\n"
    comandos_opcoes += "0) Nenhuma delas"

    # Prompt que será enviado ao ChatGPT
    prompt = f"""
Você recebeu o texto abaixo e uma lista de comandos.
Texto: "{texto}"

Lista de comandos (ID) e descrições:
{comandos_opcoes}

Dado o texto acima, qual ID de comando melhor se encaixa?
Se nenhum fizer sentido, responda apenas '0'.

ATENÇÃO: Responda somente com o número do ID exato (sem mais nada).
"""

    try:
        resposta = openai.ChatCompletion.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Você é um assistente que classifica textos em comandos pré-definidos."},
                {"role": "user", "content": prompt},
            ],
            temperature=0.0,
            max_tokens=10
        )
        conteudo_resposta = resposta.choices[0].message["content"].strip()

        # Tenta converter a resposta para inteiro.
        # Se for 0 (Nenhuma delas), retornamos None.
        # Senão, retornamos o ID escolhido.
        escolhido = int(conteudo_resposta)
        if escolhido == 0:
            return None
        else:
            return escolhido

    except Exception as e:
        print(f"[ERRO] Falha ao chamar ChatGPT: {e}")
        return None

def processar_registros():
    """
    Fluxo principal para buscar registros em logouve sem idComando,
    classificar e atualizar a tabela.
    """
    conn = get_db_connection()
    cursor = conn.cursor(dictionary=True)

    # 1) Buscar todos os registros em logouve sem idComando (nulo ou vazio).
    query_logouve = """
        SELECT * FROM logouve
        WHERE idComando IS NULL OR idComando = ''
    """
    cursor.execute(query_logouve)
    rows = cursor.fetchall()

    if not rows:
        print("Nenhum registro sem comando definido encontrado.")
        cursor.close()
        conn.close()
        return

    # 2) Buscar a lista de comandos.
    cursor_cmd = conn.cursor(dictionary=True)
    cursor_cmd.execute("SELECT id, Descricao FROM comandos")
    comandos = cursor_cmd.fetchall()
    cursor_cmd.close()

    # 3) Para cada texto sem idComando, perguntar ao ChatGPT.
    for row in rows:
        texto = row["texto"]
        logouve_id = row["id"]

        print(f"\nClassificando texto do logouve.id = {logouve_id}:")
        print(f"Texto: {texto}")

        comando_escolhido = classify_text_with_chatgpt(texto, comandos)

        if comando_escolhido is not None:
            # -------------------------------------------------
            # LÓGICA DE VERIFICAÇÃO DE DUPLICIDADE DE TEXTO
            # -------------------------------------------------
            print(f"Comando escolhido: {comando_escolhido}. Verificando duplicidade...")

            check_query = """
                SELECT id, idComando
                FROM logouve
                WHERE texto = %s
                  AND id != %s
                LIMIT 1
            """
            cursor.execute(check_query, (texto, logouve_id))
            existing_row = cursor.fetchone()

            if existing_row:
                # Já existe outro registro com mesmo texto
                existing_id = existing_row["id"]
                existing_idComando = existing_row["idComando"]

                print(f"Encontrado registro duplicado com id={existing_id}, idComando={existing_idComando}")

                # Se o idComando do duplicado for diferente, atualizamos para o novo
                if existing_idComando != comando_escolhido:
                    print("Atualizando o registro duplicado com o novo idComando...")
                    update_query = """
                        UPDATE logouve
                        SET idComando = %s
                        WHERE id = %s
                    """
                    cursor_update = conn.cursor()
                    cursor_update.execute(update_query, (comando_escolhido, existing_id))
                    conn.commit()
                    cursor_update.close()

                # Em seguida, removemos o registro atual (não queremos duplicar o texto)
                print(f"Excluindo registro atual (ID={logouve_id}) para evitar duplicidade de texto.")
                delete_query = """
                    DELETE FROM logouve
                    WHERE id = %s
                """
                cursor_delete = conn.cursor()
                cursor_delete.execute(delete_query, (logouve_id,))
                conn.commit()
                cursor_delete.close()

            else:
                # Não existe duplicado, então apenas atualizamos o idComando deste registro
                print(f"Atualizando logouve (ID={logouve_id}) com o novo comando.")
                update_query = """
                    UPDATE logouve
                    SET idComando = %s
                    WHERE id = %s
                """
                cursor_update = conn.cursor()
                cursor_update.execute(update_query, (comando_escolhido, logouve_id))
                conn.commit()
                cursor_update.close()

        else:
            # ChatGPT retornou '0' → 'Nenhuma delas'
            print(f"Nenhuma delas foi escolhida para logouve.id = {logouve_id}. Excluindo registro...")

            delete_query = """
                DELETE FROM logouve
                WHERE id = %s
            """
            cursor_delete = conn.cursor()
            cursor_delete.execute(delete_query, (logouve_id,))
            conn.commit()
            cursor_delete.close()

    cursor.close()
    conn.close()
    print("\nProcessamento concluído.")

if __name__ == "__main__":
    processar_registros()
