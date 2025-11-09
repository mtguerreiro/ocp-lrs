from setuptools import setup

setup(
    name='pyocp_lrs',
    version='0.1.0',
    description='LRS applications using OPC',
    #url='',
    author='LRS',
    author_email='marcotulio.guerreiro@gmail.com',
    packages=['pyocp_lrs'],
    install_requires=[
      'numpy',
      'scipy',
      'matplotlib',
    ],
)
