/*
 *  shaders.h
 *  glgame
 *
 *  Created by Radoslaw Mantiuk on 10/7/12.
 *  Copyright 2012 West Pomeranian University of Technology. All rights reserved.
 *
 */

#ifndef __SHADERS_H
#define __SHADERS_H

#include "conf.h"

#ifdef MAC
#include <GLUT/glut.h>
#else
#include "../3rd/glew/glew.h"
#include "../3rd/glut/glut.h"
#include <windows.h>
#endif

#include <stdio.h>
#include <iostream>

#include "util.h"

#ifdef SHADERS

class CShader {
public:
	
	static GLuint init( void ) {
		
		int IsCompiled_VS, IsCompiled_FS;
		int IsLinked;
		int maxLength;
		char *vertexInfoLog;
		char *fragmentInfoLog;
		char *shaderProgramInfoLog;
		
		/* These pointers will receive the contents of our shader source code files */
		GLchar *vertexsource, *fragmentsource;
		
		/* These are handles used to reference the shaders */
		GLuint vertexshader, fragmentshader;	
		vertexshader = glCreateShader(GL_VERTEX_SHADER);
		
		/* Read our shaders into the appropriate buffers */
		vertexsource = CUTIL::filetobuf("../shaders/phong.vert");
		fragmentsource = CUTIL::filetobuf("../shaders/phong.frag");
		
		/* Create an empty vertex shader handle */
		/* Send the vertex shader source code to GL */
		/* Note that the source code is NULL character terminated. */
		/* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
		glShaderSource(vertexshader, 1, (const GLchar**)&vertexsource, 0);
		
		/* Compile the vertex shader */
		glCompileShader(vertexshader);
		
		glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &IsCompiled_VS);
		if(IsCompiled_VS == false)
		{
			
			printf("ERROR: vertex shader not compiled\n" );
			glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &maxLength);
			
			/* The maxLength includes the NULL character */
			vertexInfoLog = (char *)malloc(maxLength);
			
			glGetShaderInfoLog(vertexshader, maxLength, &maxLength, vertexInfoLog);
			
			printf( "%d (%s)\n", maxLength, vertexInfoLog );
			
			/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
			/* In this simple program, we'll just leave */
			free(vertexInfoLog);
			return 0;
		}	
		
		/* Create an empty fragment shader handle */
		fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
		
		/* Send the fragment shader source code to GL */
		/* Note that the source code is NULL character terminated. */
		/* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
		glShaderSource(fragmentshader, 1, (const GLchar**)&fragmentsource, 0);
		
		/* Compile the fragment shader */
		glCompileShader(fragmentshader);
		
		glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &IsCompiled_FS);
		if(IsCompiled_FS == false)
		{
			printf("ERROR: fragment shader not compiled\n" );
			
			glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &maxLength);
			
			/* The maxLength includes the NULL character */
			fragmentInfoLog = (char *)malloc(maxLength);
			
			glGetShaderInfoLog(fragmentshader, maxLength, &maxLength, fragmentInfoLog);
			
			/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
			/* In this simple program, we'll just leave */
			free(fragmentInfoLog);
			return 0;
		}	
		
		/* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
		/* We must link them together to make a GL shader program */
		/* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
		/* Assign our program handle a "name" */
		GLuint shaderprogram = glCreateProgram();
		
		/* Attach our shaders to our program */
		glAttachShader(shaderprogram, vertexshader);
		glAttachShader(shaderprogram, fragmentshader);
		
		/* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
		/* Attribute locations must be setup before calling glLinkProgram. */
		glBindAttribLocation(shaderprogram, 0, "in_Position");
		glBindAttribLocation(shaderprogram, 1, "in_Color");	
		
		
		/* Link our program */
		/* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
		/* The binary code is uploaded to the GPU, if there is no error. */
		glLinkProgram(shaderprogram);
		
		/* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
		/* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
		/* too many texel fetch instructions or too many interpolators or dynamic loops. */
		
		glGetProgramiv(shaderprogram, GL_LINK_STATUS, (int *)&IsLinked);
		if(IsLinked == false)
		{
			printf("ERROR: shader not linked\n" );
			
			/* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
			glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &maxLength);
			
			/* The maxLength includes the NULL character */
			shaderProgramInfoLog = (char *)malloc(maxLength);
			
			/* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
			glGetProgramInfoLog(shaderprogram, maxLength, &maxLength, shaderProgramInfoLog);
			
			/* Handle the error in an appropriate way such as displaying a message or writing to a log file. */
			/* In this simple program, we'll just leave */
			free(shaderProgramInfoLog);
			return 0;
		}	
		
		return shaderprogram;
	}
	
	
	
	
};
#endif

#endif