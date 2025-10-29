import tkinter as tk
from styles import COLORS, FONTS

class ButtonFactory:
    """Фабрика для создания кнопок калькулятора"""
    
    @staticmethod
    def create_button(parent, text, command, button_type='number'):
        """Создает кнопку с определенным стилем"""
        colors = ButtonFactory.get_button_colors(button_type)
        
        return tk.Button(
            parent,
            text=text,
            font=FONTS['buttons'],
            bg=colors['bg'],
            fg=colors['fg'],
            command=command,
            height=2,
            width=5,
            relief='flat',
            bd=1
        )
    
    @staticmethod
    def get_button_colors(button_type):
        """Возвращает цвета для不同类型的 кнопок"""
        color_map = {
            'number': {'bg': COLORS['number_bg'], 'fg': COLORS['number_fg']},
            'operator': {'bg': COLORS['operator_bg'], 'fg': COLORS['operator_fg']},
            'special': {'bg': COLORS['special_bg'], 'fg': COLORS['special_fg']},
            'power': {'bg': COLORS['power_bg'], 'fg': COLORS['power_fg']}
        }
        return color_map.get(button_type, color_map['number'])
