import tkinter as tk
from tkinter import messagebox
import re
from buttons import ButtonFactory
from styles import COLORS, FONTS, BUTTON_LAYOUT

class Calculator:
    def __init__(self):
        self.window = tk.Tk()
        self.window.title("Калькулятор")
        self.window.geometry("300x450")
        self.window.resizable(False, False)
        self.window.configure(bg='#2C3E50')
        
        # Переменная для хранения текущего выражения
        self.expression = ""
        
        # Создание элементов интерфейса
        self.create_widgets()
    
    def create_widgets(self):
        """Создает все элементы интерфейса"""
        self.create_display()
        self.create_buttons()
        self.configure_grid()
    
    def create_display(self):
        """Создает поле ввода/вывода"""
        self.display = tk.Entry(
            self.window, 
            font=FONTS['display'],
            justify='right', 
            state='readonly',
            bg=COLORS['display_bg'],
            fg=COLORS['display_fg'],
            relief='flat',
            bd=10
        )
        self.display.grid(
            row=0, 
            column=0, 
            columnspan=4, 
            padx=10, 
            pady=10, 
            sticky='ew'
        )
        self.update_display()
    
    def create_buttons(self):
        """Создает кнопки калькулятора"""
        for row_idx, row in enumerate(BUTTON_LAYOUT, start=1):
            for col_idx, button_text in enumerate(row):
                self.create_single_button(button_text, row_idx, col_idx)
    
    def create_single_button(self, button_text, row, col):
        """Создает одну кнопку"""
        button_type = self.get_button_type(button_text)
        command = self.get_button_command(button_text)
        
        button = ButtonFactory.create_button(
            self.window, 
            button_text, 
            command, 
            button_type
        )
        
        button.grid(
            row=row,
            column=col,
            padx=2,
            pady=2,
            sticky='ew'
        )
    
    def get_button_type(self, button_text):
        """Определяет тип кнопки для стилизации"""
        if button_text in ['C', '⌫']:
            return 'special'
        elif button_text == '^':
            return 'power'
        elif button_text in ['+', '-', '*', '/', '=']:
            return 'operator'
        else:
            return 'number'
    
    def get_button_command(self, button_text):
        """Возвращает команду для кнопки"""
        commands = {
            '=': self.calculate,
            'C': self.clear,
            '⌫': self.backspace,
            '^': lambda: self.add_operator('**')
        }
        
        if button_text in commands:
            return commands[button_text]
        else:
            return lambda: self.add_to_expression(button_text)
    
    def add_to_expression(self, value):
        """Добавляет символ к выражению"""
        self.expression += str(value)
        self.update_display()
    
    def add_operator(self, operator):
        """Добавляет оператор к выражению"""
        if self.expression and not self.expression[-1] in ['+', '-', '*', '/', '.', '**']:
            self.expression += operator
            self.update_display()
    
    def clear(self):
        """Очищает выражение"""
        self.expression = ""
        self.update_display()
    
    def backspace(self):
        """Удаляет последний символ"""
        # Особенная обработка для оператора степени '**'
        if self.expression.endswith('**'):
            self.expression = self.expression[:-2]
        else:
            self.expression = self.expression[:-1]
        self.update_display()
    
    def calculate(self):
        """Вычисляет результат выражения"""
        try:
            if self.expression:
                # Заменяем отображение степени на Python-синтаксис
                expression_to_eval = self.expression
                
                # Безопасное вычисление выражения
                result = eval(expression_to_eval)
                
                # Форматируем результат
                if isinstance(result, float):
                    # Убираем лишние нули после запятой
                    result = self.format_float(result)
                
                self.expression = str(result)
                self.update_display()
                
        except ZeroDivisionError:
            messagebox.showerror("Ошибка", "Деление на ноль!")
            self.clear()
        except Exception as e:
            messagebox.showerror("Ошибка", f"Некорректное выражение!\n{str(e)}")
            self.clear()
    
    def format_float(self, number):
        """Форматирует float числа, убирая лишние нули"""
        if number.is_integer():
            return int(number)
        return number
    
    def update_display(self):
        """Обновляет отображение выражения"""
        display_text = self.expression if self.expression else "0"
        
        # Заменяем Python-синтаксис степени на красивый вид для отображения
        display_text = display_text.replace('**', '^')
        
        self.display.config(state='normal')
        self.display.delete(0, tk.END)
        self.display.insert(0, display_text)
        self.display.config(state='readonly')
    
    def configure_grid(self):
        """Настраивает веса строк и столбцов для растягивания"""
        for i in range(6):  # 5 рядов кнопок + 1 ряд дисплея
            self.window.grid_rowconfigure(i, weight=1)
        for i in range(4):
            self.window.grid_columnconfigure(i, weight=1)
    
    def run(self):
        """Запускает приложение"""
        self.window.mainloop()
