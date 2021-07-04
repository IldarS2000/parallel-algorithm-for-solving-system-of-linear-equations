import openpyxl


def writeToExcel(fileName, column, row):
    try:
         file = open(fileName, 'r')
    except IOError:
         print('file error')

    while True:
        value = file.readline()
        if not value:
            break
        spreadsheet[column + str(row)] = float(value)
        row += 1

    file.close()

# открываем таблицу
book = openpyxl.Workbook()
spreadsheet = book.active

# записываем из файлов в таблицу
column = 'A'
spreadsheet['A1'] = 'size'
writeToExcel('nEquations.txt', column, 2)

spreadsheet[chr(ord(column) + 1) + '1'] = str(1) + ' поток'
writeToExcel('runtimeWith' + str(1) + 'Threads.txt', chr(ord(column) + 1), 2)

spreadsheet[chr(ord(column) + 2) + '1'] = str(2) + ' потока'
writeToExcel('runtimeWith' + str(2) + 'Threads.txt', chr(ord(column) + 2), 2)

spreadsheet[chr(ord(column) + 3) + '1'] = str(4) + ' потока'
writeToExcel('runtimeWith' + str(4) + 'Threads.txt', chr(ord(column) + 3), 2)

spreadsheet[chr(ord(column) + 4) + '1'] = str(8) + ' потоков'
writeToExcel('runtimeWith' + str(8) + 'Threads.txt', chr(ord(column) + 4), 2)

spreadsheet[chr(ord(column) + 5) + '1'] = str(12) + ' потоков'
writeToExcel('runtimeWith' + str(12) + 'Threads.txt', chr(ord(column) + 5), 2)

spreadsheet[chr(ord(column) + 6) + '1'] = str(16) + ' потоков'
writeToExcel('runtimeWith' + str(16) + 'Threads.txt', chr(ord(column) + 6), 2)


# сохраняем таблицу
book.save('graph.xlsx')
print('txt files have been parsed, spreadsheet is ready!')

