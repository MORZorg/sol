#include "dataIO.h"

PyObject* gui_instance;
int gui_initialized = GUI_NO_INIT;

void initialize_gui(void)
{
	PyObject* gui_name;
	PyObject* gui_module;
	PyObject* gui_class;

	Py_Initialize();

	// Bad but works... Always better than using wchar_t
	PyRun_SimpleString("import sys; sys.path.append('.')\n");

	gui_name = PyUnicode_FromString(PYTHON_MODULE_NAME);
	gui_module = PyImport_Import(gui_name);
	Py_DECREF(gui_name);

	if (gui_module == NULL)
		printf("GUI module not found! Check your Python3/PyQt5 config.\n");

	gui_class = PyObject_GetAttrString(gui_module, PYTHON_CLASS_NAME);
	Py_DECREF(gui_module);

	gui_instance = PyEval_CallObject(gui_class, NULL);
	Py_DECREF(gui_class);

	gui_initialized = GUI_EXT_INIT;
}

void finalize_gui(void)
{
	Py_DECREF(gui_instance);

	Py_Finalize();

	gui_initialized = GUI_NO_INIT;
}

ByteArray userInput(char* schema)
{
	PyObject* gui_function;
	PyObject* gui_args;
	PyObject* gui_result;
	long success;
	ByteArray result;

	if( !gui_initialized )
	{
		initialize_gui();
		gui_initialized = GUI_SELF_INIT;
	}

	gui_function = PyObject_GetAttrString(gui_instance,
										  PYTHON_REQUEST_INPUT_NAME);
	gui_args = PyTuple_Pack(1, PyUnicode_FromString(schema));
	gui_result = PyObject_CallObject(gui_function, gui_args);
	Py_DECREF(gui_args);
	Py_DECREF(gui_function);

	success = PyLong_AsLong(PyTuple_GetItem(gui_result, 0));
	PyBytes_AsStringAndSize(PyTuple_GetItem(gui_result, 1),
							&result.value, &result.size);
	Py_DECREF(gui_result);

	if( gui_initialized == GUI_SELF_INIT )
		finalize_gui();

	// TODO It could be possible to find a way to propagate the error and
	// manage it (eg: stop the VM nicely).
	// The cause almost surely is the user closing the dialog instead of
	// pressing OK, so his intent should simply be to stop the execution.
	if (success == 0)
		exit(1);
	else
		return result;
}

void userOutput(char* schema, ByteArray data)
{
	PyObject* gui_function;
	PyObject* gui_args;
	PyObject* gui_result;
	long result;

	if( !gui_initialized )
	{
		initialize_gui();
		gui_initialized = GUI_SELF_INIT;
	}

	gui_function = PyObject_GetAttrString(gui_instance,
										  PYTHON_REQUEST_OUTPUT_NAME);
	gui_args =
		PyTuple_Pack(2,
					 PyUnicode_FromString(schema),
					 PyBytes_FromStringAndSize(data.value, data.size));
	gui_result = PyObject_CallObject(gui_function, gui_args);
	Py_DECREF(gui_args);
	Py_DECREF(gui_function);

	result = PyLong_AsLong(gui_result);
	Py_DECREF(gui_result);

	if( gui_initialized == GUI_SELF_INIT )
		finalize_gui();

	if (result == 0)
		exit(1);
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
