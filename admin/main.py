from kivy.app import App
from kivy.uix.gridlayout import GridLayout
from kivy.uix.button import Button
from kivy.uix.label import Label
from kivy.uix.textinput import TextInput

class InterfaceManager(GridLayout):
    def __init__(self, **kwargs):
        super(InterfaceManager, self).__init__(**kwargs)
        self.mainLayout = GridLayout(cols=1)
        button = Button(text='Calibrar celda')
        button.bind(on_press=self.onCalibrateClick)
        self.mainLayout.add_widget(button)
        button = Button(text='Cambiar velocidad de cuchilla')
        button.bind(on_press=self.onVelocityClick)
        self.mainLayout.add_widget(button)
        self.add_widget(self.mainLayout)
    
        self.calibrateLayout = GridLayout(cols=2)
        button = Button(text='Volver')
        button.bind(on_press=self.onHomeClick)
        self.calibrateLayout.add_widget(button)

        self.velocityLayout = GridLayout(cols=2)
        label = Label(text="Velocidad")
        self.velocityLayout.add_widget(label)
        txtVel = TextInput()
        self.velocityLayout.add_widget(txtVel)
        button = Button(text='Volver')
        button.bind(on_press=self.onHomeClick)
        self.velocityLayout.add_widget(button)
        button = Button(text='Enviar')
        button.bind(on_press=self.onHomeClick)
        self.velocityLayout.add_widget(button)
        

    def onCalibrateClick(self, instance):
        self.clear_widgets()
        self.add_widget(self.calibrateLayout)

    def onVelocityClick(self, instance):
        self.clear_widgets()
        self.add_widget(self.velocityLayout)

    def onHomeClick(self, instance):
        self.clear_widgets()
        self.add_widget(self.mainLayout)


class MainApp(App):
    def build(self):
        return InterfaceManager(cols=1)

MainApp().run()