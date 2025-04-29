import requests
import pandas as pd
import time

try:
    latest_block_url = "https://blockstream.info/api/blocks/tip/height"
    latest_block_response = requests.get(latest_block_url)
    latest_block_response.raise_for_status()
    latest_block_height = int(latest_block_response.text)

    blocks_per_day = 144
    start_block_height = latest_block_height - (2 * blocks_per_day)
    block_heights = list(range(latest_block_height, start_block_height - 1, -1))

    transactions = []

    for block_height in block_heights:
        try:
            block_hash_url = f"https://blockstream.info/api/block-height/{block_height}"
            block_hash_response = requests.get(block_hash_url)
            block_hash_response.raise_for_status()
            block_hash = block_hash_response.text

            txs_url = f"https://blockstream.info/api/block/{block_hash}/txs"
            txs_response = requests.get(txs_url)
            txs_response.raise_for_status()
            tx_hashes = txs_response.json()

            for tx_info in tx_hashes:
                tx_hash = tx_info['txid']
                tx_url = f"https://blockstream.info/api/tx/{tx_hash}"
                tx_response = requests.get(tx_url)
                tx_response.raise_for_status()
                tx_data = tx_response.json()

                for input_data in tx_data["vin"]:
                    if "is_coinbase" in input_data and input_data["is_coinbase"]:
                        continue
                    if "prevout" in input_data and "scriptpubkey_address" in input_data["prevout"]:
                        input_address = input_data["prevout"]["scriptpubkey_address"]
                        for output_data in tx_data["vout"]:
                            if "scriptpubkey_address" in output_data:
                                output_address = output_data["scriptpubkey_address"]
                                transactions.append(
                                    {
                                        "source": input_address,
                                        "destination": output_address,
                                        "amount": output_data["value"],
                                    }
                                )

            time.sleep(1)

        except requests.exceptions.RequestException as e:
            print(f"Error: {e}")
        except ValueError as e:
            print(f"Error: Invalid JSON response: {e}")

    df = pd.DataFrame(transactions)
    output_filename = "bitcoin_transactions_last_2_days.csv"
    df.to_csv(output_filename, index=False)
    print(f"CSV file '{output_filename}' created successfully!")

except requests.exceptions.RequestException as e:
    print(f"Error: {e}")
except ValueError as e:
    print(f"Error: Invalid JSON response: {e}")