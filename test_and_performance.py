# # Objetivo

# El objetivo de este proyecto es aprender a modelar un problema en CNF, y a usar un SAT solver para resolverlo, así como traducir la salida del SAT solver a un formato legible.
# No solo se evaluará que la implementación funcione, sino la eficiencia de su traducción a CNF del problema.

# # Problema a resolver

# Imagine que se está organizando un torneo, y se le pide realizar un programa que encuentre una asignación de fecha y hora en la que los juegos van a ocurrir. Las reglas son las siguientes:

# * Todos los participantes deben jugar dos veces con cada uno de los otros participantes, una como "visitantes" y la otra como "locales". Esto significa que, si hay 10 equipos, cada equipo jugará 18 veces.
# * Dos juegos no pueden ocurrir al mismo tiempo.
# * Un participante puede jugar a lo sumo una vez por día.
# * Un participante no puede jugar de "visitante" en dos días consecutivos, ni de "local" dos días seguidos.
# * Todos los juegos deben empezar en horas "en punto" (por ejemplo, las 13:00:00 es una hora válida pero las 13:30:00 no).
# * Todos los juegos deben ocurrir entre una fecha inicial y una fecha final especificadas. Pueden ocurrir juegos en dichas fechas.
# * Todos los juegos deben ocurrir entre un rango de horas especificado, el cuál será fijo para todos los días del torneo.
# * A efectos prácticos, todos los juegos tienen una duración de dos horas.

# # Formato de entrada

# Su sistema debe recibir un JSON con el siguiente formato (asuma que siempre recibirá el formato correcto):

# ```
# {
#   "tournament_name": String. Nombre del torneo,
#   "start_date": String. Fecha de inicio del torneo en formato ISO 8601,
#   "end_date": String. Fecha de fin del torneo en formato ISO 8601,
#   "start_time": String. Hora a partir de la cuál pueden ocurrir los juegos en cada día, en formato ISO 8601,
#   "end_time": String. Hora hasta la cuál pueden ocurrir los juegos en cada día, en formato ISO 8601,
#   "participants": [String]. Lista con los nombres de los participantes en el torneo
# }
# ```

# Asuma que todas las horas vienen sin zona horaria especificada, y asuma por lo tanto que su zona horaria es UTC.

# # Actividad 1

# Deben crear una traducción del problema a formato CNF, y luego deben crear un programa, en el lenguaje de programación que sea de su agrado, que traduzca cualquier JSON en el formato propuesto a la representación del problema en formato [DIMACS CNF](https://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html)

# # Actividad 2

# Usando la transformación creada en la parte anterior, los archvios en formato DIMACS CNF pueden ser usados como entrada para el SAT solver [Glucose](https://www.labri.fr/perso/lsimon/glucose/). Debe crear un programa, en el lenguaje de programación que sea de su agrado, que traduzca la salida de Glucose al resolver el problema en un archivo con el mismo nombre del torneo y extensión `.ics`, en formato de [iCalendar](https://en.wikipedia.org/wiki/ICalendar) de manera que sea posible agregar la asignación de los juegos a un gestor de calendarios. Para ello puede usar cualquier librería que considere necesaria. Los eventos del calendario deben tener ocurrir a la hora que fue asignada cumpliendo todas las reglas dadas, y deben indicar quiénes son los participantes en el juego, quién es el "local" y quién es el "visitante".

# # Actividad 3

# Debe crear un cliente que maneje todo el proceso. Es decir, reciba un JSON en el formato de entrada, ejecute el programa que lo transforma en CNF, introduzca el resultado  en Glucose, y se asegure de que se cree el archivo .ics con la respuesta, o falle en caso de ser UNSAT. Debe generar casos de prueba fáciles y difíciles, y medir el rendimiento de su solución.

import os
import json
import datetime
import subprocess
import pandas as pd
import sys

# generate test cases


def generate_test_cases(num_participants=[2, 4, 5], date_range=[4, 5, 6], time_range=[2, 3, 4], add_info_name="easy") -> dict:
    """
    Generate easy test cases for the tournament problem

    Parameters
    ----------
    num_participants : list
        List of number of participants for the tournament
    date_range : list
        List of number of days for the tournament
    time_range : list
        List of number of hours for the tournament

    Returns
    -------
    test_cases : dict
        Dictionary with the test cases
    """
    # number of cases, is the product of the number of participants, dates and times
    # num_cases = len(num_participants) * len(date_range) * len(time_range)
    test_cases = []
    for num_participant in num_participants:
        for date in date_range:
            # 2021-05-01 in ISO 8601
            current_date = datetime.datetime(2021, 5, 1)
            # sum the number of days to the current date
            end_date = current_date + datetime.timedelta(days=date)
            for time in time_range:
                # 00:00 AM in ISO 8601
                current_time = datetime.datetime(2021, 5, 1, 0)
                # sum the number of hours to the current time
                end_time = current_time + datetime.timedelta(hours=time)
                test_cases.append({
                    "tournament_name": f"tournament_{add_info_name}_{num_participant}_{date}_{time}",
                    "start_date": current_date.date().isoformat(),
                    "end_date": end_date.date().isoformat(),
                    "start_time": current_time.time().isoformat(),
                    "end_time": end_time.time().isoformat(),
                    "participants": [f"participant_{i}" for i in range(num_participant)]
                })

    return test_cases


def measure_runtime(test_dir: str, output_dir: str, result_dir: str, name: str):
    """
    Measure the runtime of the test cases

    Parameters
    ----------
    test_dir : str
        Directory where the test cases are located
    output_dir : str
        Directory where the output files are located

    Returns
    -------
    runtimes : dict
        Dictionary with the runtime of each test case
    """
    # get the test cases
    test_cases = os.listdir(test_dir)
    # create a dataframe to save the runtimes
    runtimes = pd.DataFrame(columns=["test_case", "runtime"])
    # create the output directory if it does not exist
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # iterate over the test cases
    for test_case in test_cases:
        print(f"Running test case {test_case}")
        # execute time command to measure the runtime
        try:
            runtime = subprocess.run(["time", "-p", "./tournement-SAT",
                                      f"{test_dir}/{test_case}", f"{output_dir}/{test_case.replace('.json', '')}"], capture_output=True, timeout=60*15)
        except subprocess.TimeoutExpired:
            print(f"Timeout for test case {test_case}")
            continue
        # get the runtime
        runtime = runtime.stderr.decode("utf-8").split("\n")[1].split(" ")[1]
        # save the runtime
        runtimes = pd.concat([runtimes, pd.DataFrame(
            [[test_case, runtime]], columns=["test_case", "runtime"])])

    # create the result directory if it does not exist
    if not os.path.exists(result_dir):
        os.makedirs(result_dir)
    # save the runtimes in csv file
    runtimes.to_csv(f"{result_dir}/{name}.csv", index=False)


def create_test_files(test_cases, root_folder, name):
    if not os.path.exists(f"{root_folder}/{name}"):
        os.makedirs(f"{root_folder}/{name}")
    for test_case in test_cases:
        # save the test case
        with open(f"{root_folder}/{name}/{test_case['tournament_name']}.json", "w") as file:
            json.dump(test_case, file)


if __name__ == "__main__":
    # if len(sys.argv) == 1:
    #     # delete easy and hard test cases
    #     # if os.path.exists("test/easy"):
    #     #     subprocess.run(["rm", "-rf", "test/easy"])
    #     if os.path.exists("test/hard"):
    #         subprocess.run(["rm", "-rf", "test/hard"])

    # detect if executable if created
    if not os.path.exists("tournement-SAT"):
        # run make to create the executable
        subprocess.run(["make"])

    # generate easy test cases
    easy_test_cases = generate_test_cases()
    # generate hard test cases (6,8,6)
    hard_test_cases = generate_test_cases(num_participants=[6, 7, 8], date_range=[
                                          8, 9], time_range=[4], add_info_name="hard")
    # save the test cases in folder test/easy and test/hard
    root_folder = "test"
    # create the ease test cases
    create_test_files(easy_test_cases, root_folder, "easy")
    # create the hard test cases
    create_test_files(hard_test_cases, root_folder, "hard")
    # measure the runtime of the easy test cases
    measure_runtime("test/easy", "output/easy", "result", "easy")
    # measure the runtime of the hard test cases
    measure_runtime("test/hard", "output/hard", "result", "hard")
