#include "dataIO.h"

PyObject* gui_module;
int gui_initialized = GUI_NO_INIT;

void initialize_gui(void)
{
	Py_Initialize();

    // Bad but works... Always better than using wchar_t
    PyRun_SimpleString("import sys; sys.path.append('.')\n");

	gui_module = PyImport_Import(PyUnicode_FromString(PYTHON_MODULE_NAME));

	gui_initialized = GUI_EXT_INIT;
}

void finalize_gui(void)
{
	Py_Finalize();
	gui_initialized = GUI_NO_INIT;
}

ByteArray userInput(char* schema)
{
	fprintf( stderr, "----------------STILL WORKING\n" );
	PyObject* gui_function;
	fprintf( stderr, "----------------STILL WORKING\n" );
	PyObject* gui_args;
	fprintf( stderr, "----------------STILL WORKING\n" );
	ByteArray result;
	fprintf( stderr, "----------------STILL WORKING %d\n", !gui_initialized );

	if( !gui_initialized )
	{
		initialize_gui();
		fprintf( stderr, "----------------STILL WORKING\n" );
		gui_initialized = GUI_SELF_INIT;
		fprintf( stderr, "----------------STILL WORKING\n" );
	}
	
	fprintf( stderr, "----------------STILL WORKING\n" );

	gui_function = PyObject_GetAttrString(gui_module,
										  PYTHON_REQUEST_INPUT_NAME);
	fprintf( stderr, "----------------STILL WORKING\n" );
	gui_args = PyTuple_Pack(1, PyUnicode_FromString(schema));
	fprintf( stderr, "----------------STILL WORKING %d %d %s\n", gui_function == NULL, gui_args == NULL, schema );

	PyObject* a = PyObject_CallObject( gui_function, gui_args );
	fprintf( stderr, "----------------STILL WORKING\n" );

	PyBytes_AsStringAndSize(
		a,
		&result.value,
		&result.size);
	fprintf( stderr, "----------------STILL WORKING %d\n", gui_initialized == GUI_SELF_INIT );

	if( gui_initialized == GUI_SELF_INIT )
		finalize_gui();
	fprintf( stderr, "----------------STILL WORKING\n" );

	return result;
}

void userOutput(char* schema, ByteArray data)
{
	fprintf( stderr, "----------------STILL WORKING\n" );
	PyObject* gui_function;
	fprintf( stderr, "----------------STILL WORKING\n" );
	PyObject* gui_args;
	fprintf( stderr, "----------------STILL WORKING %d\n", !gui_initialized );

	if( !gui_initialized )
	{
		initialize_gui();
		fprintf( stderr, "----------------STILL WORKING\n" );
		gui_initialized = GUI_SELF_INIT;
		fprintf( stderr, "----------------STILL WORKING\n" );
	}

	gui_function = PyObject_GetAttrString(gui_module,
										  PYTHON_REQUEST_OUTPUT_NAME);
	fprintf( stderr, "----------------STILL WORKING\n" );
	gui_args =
		PyTuple_Pack(2,
					 PyUnicode_FromString(schema),
					 PyBytes_FromStringAndSize(data.value, data.size));
	fprintf( stderr, "----------------STILL WORKING\n" );

	PyObject_CallObject(gui_function, gui_args);
	fprintf( stderr, "----------------STILL WORKING %d\n", gui_initialized == GUI_SELF_INIT );

	if( gui_initialized == GUI_SELF_INIT )
		finalize_gui();
	fprintf( stderr, "----------------STILL WORKING\n" );
}

ByteArray fileInput(char* filename)
{
	FILE* file = fopen(filename, "r");

	ByteArray result;
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		result.size = ftell(file);
		rewind(file);

		result.value = malloc(result.size + 1);
		fread(result.value, result.size, 1, file);
		fclose(file);

		result.value[result.size] = 0;
		return result;
	}
	else
	{
		result.size = 0;
		result.value = NULL;
		return result;
	}
}

void fileOutput(char* filename, ByteArray data)
{
	FILE* file = fopen(filename, "w");

	if (file != NULL)
	{
		fwrite(data.value, sizeof( byte ), data.size, file);
		fclose(file);
	}
}
