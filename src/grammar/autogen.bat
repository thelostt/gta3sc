@echo off
set CLASSPATH=!CLASSPATH!^;../../deps/antlr-3.5.2.jar
java org.antlr.Tool gta3script.g -o autogen