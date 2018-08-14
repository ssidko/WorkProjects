
digit_0 = [' *** ',
           '*   *',
           '*  **',
           '* * *',
           '**  *',
           '*   *',
           ' *** ']

digit_1 = ['  *  ',
           ' **  ',
           '  *  ',
           '  *  ',
           '  *  ',
           '  *  ',
           ' *** ']

digit_2 = [' *** ',
           '*   *',
           '*   *',
           '   * ',
           '  *  ',
           ' *   ',
           '*****']

digit_3 = [' *** ',
           '*   *',
           '    *',
           '  ** ',
           '    *',
           '*   *',
           ' *** ']

digit_4 = ['    *',
           '   **',
           '  * *',
           ' *  *',
           '*   *',
           '*****',
           '    *']

digit_5 = ['*****',
           '*    ',
           '*    ',
           '**** ',
           '    *',
           '    *',
           '**** ']

digit_6 = ['   * ',
           '  *  ',
           ' *   ',
           '**** ',
           '*   *',
           '*   *',
           ' *** ']

digit_7 = ['*****',
           '    *',
           '   * ',
           '  *  ',
           ' *   ',
           '*    ',
           '*    ']

digit_8 = [' *** ',
           '*   *',
           '*   *',
           ' *** ',
           '*   *',
           '*   *',
           ' *** ']

digit_9 = [' *** ',
           '*   *',
           '*   *',
           ' ****',
           '   * ',
           '  *  ',
           ' *   ']


digits = [digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8, digit_9]


def PrintDigitalNumber(number):
    number_str = str(number)
    for line in range(len(digits[0])):
        line_str = ''
        for n in number_str:
            '''
            for c in  digits[int(n)][line]:
                if c == '*':
                    line_str += n
                else:
                    line_str += " "
            line_str += " "
            '''
            line_str += digits[int(n)][line] + " "
        print(line_str)

#PrintDigitalNumber('158')

