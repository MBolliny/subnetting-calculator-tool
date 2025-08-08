import math
from math import ceil, log2
import ipaddress

'''
500: {  # ← CHIAVE: numero host richiesti
    'reti': ['LAN_Uffici', 'LAN_Produzione'],
    'bit_host': 9,
    'netmask': 23,
    'salto': 512,
}
'''

# creo chiave del nuovo dizionario CHIAVE:VALORE
def crea_dizionario(diz_input):
    diz_output = {}

    # per ogni elementi del dizionario delle reti di ingresso
    for input_key,input_value in diz_input.items():

        # se esiste già un dizionario con stesso nome del input_value
        if input_value in diz_output:
            # aggiurno solo campo delle liste
            diz_output[input_value]['lista_reti'].append(input_key)

        # senno devo creare questo dizionario
        else:
            # prima calcoli
            bit_host = ceil(log2(input_value + 2))  # +2 per network e broadcast
            netmask = 32 - bit_host
            salto = pow(2, bit_host)

            # poi crei il dizionario con i valori giusti
            diz_valore = {
                'lista_reti': [input_key],
                'bit_host': bit_host,
                'netmask': netmask,
                'salto': salto
            }

            # creazione diz OUTPUT -> input_value : diz_valore
            diz_output[input_value] = diz_valore

    return diz_output


def calcolaSubnet(input_diz, prima_rete):
    ip_corrente = ipaddress.IPv4Address(prima_rete)
    livello = 0

    for chiave, valore in input_diz.items():
        lenght = len(valore['lista_reti'])
        j = 0

        indent = "  " * livello  # Indentazione corrente

        # Stampa le reti del gruppo corrente
        for i in range(lenght):
            nuova_ip = ip_corrente + (i * valore['salto'])
            print(f"{indent}├─ {valore['lista_reti'][j]}:  {nuova_ip} /{valore['netmask']}")
            j += 1

        # Calcola il prossimo IP disponibile
        ip_corrente = ip_corrente + (lenght * valore['salto'])

        # Stampa il prossimo IP disponibile (indentato)
        print(f"{indent}├─ {ip_corrente}")

        # Incrementa il livello di indentazione per il prossimo gruppo
        livello += 1
