import random


def generate_temperature_data(day, month, year):
    min = 40
    max = 80
    data = {
        'day': f'{day:02}',
        'month': f'{month:02}',
        'year': str(year),
        '0h': f'{random.uniform(min, max):.2f}',
        '1h': f'{random.uniform(min, max):.2f}',
        '2h': f'{random.uniform(min, max):.2f}',
        '3h': f'{random.uniform(min, max):.2f}',
        '4h': f'{random.uniform(min, max):.2f}',
        '5h': f'{random.uniform(min, max):.2f}',
        '6h': f'{random.uniform(min, max):.2f}',
        '7h': f'{random.uniform(min, max):.2f}',
        '8h': f'{random.uniform(min, max):.2f}',
        '9h': f'{random.uniform(min, max):.2f}',
        '10h': f'{random.uniform(min, max):.2f}',
        '11h': f'{random.uniform(min, max):.2f}',
        '12h': f'{random.uniform(min, max):.2f}',
        '13h': f'{random.uniform(min, max):.2f}',
        '14h': f'{random.uniform(min, max):.2f}',
        '15h': f'{random.uniform(min, max):.2f}',
        '16h': f'{random.uniform(min, max):.2f}',
        '17h': f'{random.uniform(min, max):.2f}',
        '18h': f'{random.uniform(min, max):.2f}',
        '19h': f'{random.uniform(min, max):.2f}',
        '20h': f'{random.uniform(min, max):.2f}',
        '21h': f'{random.uniform(min, max):.2f}',
        '22h': f'{random.uniform(min, max):.2f}',
        '23h': f'{random.uniform(min, max):.2f}',
    }
    return data


def generate_insert_statement(data):
    fields = ', '.join(data.keys())
    values = ', '.join(f"'{value}'" for value in data.values())
    return f"INSERT INTO humidity_air ({fields}) VALUES ({values});"


insert_statements = []

month = 7
year = 2024
for day in range(1, 19):
    data = generate_temperature_data(day, month, year)
    insert_statements.append(generate_insert_statement(data))

for statement in insert_statements:
    print(statement)
