from fontTools.misc.arrayTools import pointInRect

from functions import crea_dizionario, calcolaSubnet, print_diz
import ipaddress

def main():

    # dati iniziali
    rete = "131.175.184.0"
    netmask = 21
    host = {
        "netA": 500,
        "netB": 500,
        "netC": 220,
        "netD": 220,
        "netE": 120,
        "netF": 120,
        "netG": 120,
        "netH": 60,
        "netI": 28,
        "PP1" : 1,
        "PP2": 1,
        "PP3": 1,
        "PP4": 1
    }

    # organizzazione crescente
    sorted_rete = dict(sorted(host.items(), key=lambda item: item[1], reverse=True))

    # creazione dizionario di lavoro
    result_dizt = crea_dizionario(sorted_rete)
    print_diz(result_dizt)

    # calcolo subnet
    print(f"\nSubnetting di {rete} /{netmask}")
    print("-" * 40)
    calcolaSubnet(result_dizt, rete)

if __name__ == "__main__":
    main()