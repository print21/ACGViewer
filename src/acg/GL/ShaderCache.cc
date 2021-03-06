/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
\*===========================================================================*/




#include "ShaderCache.hh"

#include <cstdio>
#include <iostream>
#include <fstream>


#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include <ACG/GL/GLError.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>


namespace ACG
{

ShaderCache::ShaderCache():
    cache_(),
    cacheStatic_(),
    timeCheck_(false)
{
}

ShaderCache::~ShaderCache()
{
  // free all glsl programs in cache
  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
    delete it->second;

  for (CacheList::iterator it = cacheStatic_.begin(); it != cacheStatic_.end();  ++it)
    delete it->second;

  for (CacheList::iterator it = cacheComputeShaders_.begin(); it != cacheComputeShaders_.end();  ++it)
    delete it->second;
}

ShaderCache* ACG::ShaderCache::getInstance()
{
  static ShaderCache singleton;
  return &singleton;
}


GLSL::Program* ACG::ShaderCache::getProgram( const ShaderGenDesc* _desc )
{
  std::vector<unsigned int> dummy;
  return getProgram(_desc, dummy);
}

//***********************************************************************
// TODO implement binary search eventually (if cache access is getting too slow)
// - modify compareShaderGenDescs s.t. it defines an order
// or generate a hash key from ShaderGenDesc

GLSL::Program* ACG::ShaderCache::getProgram( const ShaderGenDesc* _desc, const std::vector<unsigned int>& _mods )
{
  CacheEntry newEntry;
  newEntry.desc = *_desc;
  newEntry.mods = _mods;

  if (!_desc->fragmentTemplateFile.isEmpty())
  {
    newEntry.strFragmentTemplate = _desc->fragmentTemplateFile;
    newEntry.fragmentFileLastMod = QFileInfo(newEntry.strFragmentTemplate).lastModified();
  }

  if (!_desc->tessControlTemplateFile.isEmpty())
  {
    newEntry.strTessControlTemplate = _desc->tessControlTemplateFile;
    newEntry.tessControlFileLastMod = QFileInfo(newEntry.strTessControlTemplate).lastModified();
  }

  if (!_desc->tessEvaluationTemplateFile.isEmpty())
  {
    newEntry.strTessEvaluationTemplate = _desc->tessEvaluationTemplateFile;
    newEntry.tessEvaluationFileLastMod = QFileInfo(newEntry.strTessEvaluationTemplate).lastModified();
  }

  if (!_desc->geometryTemplateFile.isEmpty())
  {
      newEntry.strGeometryTemplate = _desc->geometryTemplateFile;
      newEntry.geometryFileLastMod = QFileInfo(newEntry.strGeometryTemplate).lastModified();
  }

  if (!_desc->vertexTemplateFile.isEmpty())
  {
    newEntry.strVertexTemplate = _desc->vertexTemplateFile;
    newEntry.vertexFileLastMod = QFileInfo(newEntry.strVertexTemplate).lastModified();
  }

  CacheList::iterator oldCache = cache_.end();

  for (CacheList::iterator it = cache_.begin(); it != cache_.end();  ++it)
  {
    // If the shaders are equal, we return the cached entry
    if (!compareShaderGenDescs(&it->first, &newEntry))
    {
      if ( timeCheck_ && !compareTimeStamp(&it->first, &newEntry))
        oldCache = it;
      else
        return it->second;
    }
  }

  // glsl program not in cache, generate shaders
  ShaderProgGenerator progGen(_desc, _mods);

  if (!dbgOutputDir_.isEmpty())
  {
    static int counter = 0;

    QString fileName = QString("shader_%1.glsl").arg(counter++,2,10,QLatin1Char('0')) ;
    fileName = dbgOutputDir_ + QDir::separator() + fileName;

    QFile fileOut(fileName);
    if (fileOut.open(QFile::WriteOnly | QFile::Truncate))
    {
      QTextStream outStrm(&fileOut);

      outStrm << _desc->toString();
      outStrm << "\nmods: ";

      for (size_t i = 0; i < _mods.size(); ++i)
        outStrm << _mods[i] << (i+1 < _mods.size() ? ", " : "");
      outStrm << "\n";


      outStrm << "\n---------------------vertex-shader--------------------\n\n";
      
      for (int i = 0; i < progGen.getVertexShaderCode().size(); ++i)
        outStrm << progGen.getVertexShaderCode()[i] << "\n";

      if (progGen.hasTessControlShader())
      {
        outStrm << "\n---------------------tesscontrol-shader--------------------\n\n";

        for (int i = 0; i < progGen.getTessControlShaderCode().size(); ++i)
          outStrm << progGen.getTessControlShaderCode()[i] << "\n";
      }

      if (progGen.hasTessEvaluationShader())
      {
        outStrm << "\n---------------------tesseval-shader--------------------\n\n";

        for (int i = 0; i < progGen.getTessEvaluationShaderCode().size(); ++i)
          outStrm << progGen.getTessEvaluationShaderCode()[i] << "\n";
      }

      if (progGen.hasGeometryShader())
      {
        outStrm << "\n---------------------geometry-shader--------------------\n\n";

        for (int i = 0; i < progGen.getGeometryShaderCode().size(); ++i)
          outStrm << progGen.getGeometryShaderCode()[i] << "\n";
      }

      outStrm << "\n---------------------fragment-shader--------------------\n\n";

      for (int i = 0; i < progGen.getFragmentShaderCode().size(); ++i)
        outStrm << progGen.getFragmentShaderCode()[i] << "\n";


      fileOut.close();
    }
  }

  GLSL::FragmentShader* fragShader = new GLSL::FragmentShader();
  GLSL::VertexShader* vertShader   = new GLSL::VertexShader();

  vertShader->setSource(progGen.getVertexShaderCode());
  fragShader->setSource(progGen.getFragmentShaderCode());

  vertShader->compile();
  fragShader->compile();

  GLSL::Program* prog = new GLSL::Program();
  prog->attach(vertShader);
  prog->attach(fragShader);

  // Check if we have a geometry shader and if we have support for it, enable it here
  if ( progGen.hasGeometryShader() ) {
    GLSL::GeometryShader* geomShader = new GLSL::GeometryShader();
    geomShader->setSource(progGen.getGeometryShaderCode());
    geomShader->compile();
    prog->attach(geomShader);
  }

  // Check if we have tessellation shaders and if we have support for it, enable it here
  if ( progGen.hasTessControlShader() || progGen.hasTessEvaluationShader() ) {
    GLSL::Shader* tessControlShader = 0, *tessEvalShader = 0;

#ifdef GL_ARB_tessellation_shader
    tessControlShader = new GLSL::TessControlShader();
    tessEvalShader = new GLSL::TessEvaluationShader();
#endif // GL_ARB_tessellation_shader

    if (tessControlShader && progGen.hasTessControlShader())
    {
      tessControlShader->setSource(progGen.getTessControlShaderCode());
      tessControlShader->compile();
      prog->attach(tessControlShader);
    }

    if (tessEvalShader && progGen.hasTessEvaluationShader())
    {
      tessEvalShader->setSource(progGen.getTessEvaluationShaderCode());
      tessEvalShader->compile();
      prog->attach(tessEvalShader);
    }
    
  }

  prog->link();
  glCheckErrors();

  if (oldCache != cache_.end())
  {
    if (!prog->isLinked())
    {
      delete prog;
      return oldCache->second;
    }
    else
    {
      cache_.erase(oldCache);
    }
  }

  cache_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

  return prog;
}

GLSL::Program* ACG::ShaderCache::getProgram( const char* _vertexShaderFile, 
  const char* _tessControlShaderFile,
  const char* _tessEvalShaderFile,
  const char* _geometryShaderFile,
  const char* _fragmentShaderFile,
  QStringList* _macros, bool _verbose )
{
  CacheEntry newEntry;


  // store filenames and timestamps in new entry

  // fragment shader
  QFileInfo fileInfo(_fragmentShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_fragmentShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strFragmentTemplate = absFilename;
    newEntry.fragmentFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strFragmentTemplate = _fragmentShaderFile;
    newEntry.fragmentFileLastMod = fileInfo.lastModified();
  }

  // vertex shader
  fileInfo = QFileInfo(_vertexShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_vertexShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strVertexTemplate = absFilename;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strVertexTemplate = _vertexShaderFile;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }


  // geometry shader
  if (_geometryShaderFile)
  {
    fileInfo = QFileInfo(_geometryShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_geometryShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strGeometryTemplate = absFilename;
      newEntry.geometryFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strGeometryTemplate = _geometryShaderFile;
      newEntry.geometryFileLastMod = fileInfo.lastModified();
    }
  }

  // tess-ctrl shader
  if (_tessControlShaderFile)
  {
    fileInfo = QFileInfo(_tessControlShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_tessControlShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strTessControlTemplate = absFilename;
      newEntry.tessControlFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strTessControlTemplate = _tessControlShaderFile;
      newEntry.tessControlFileLastMod = fileInfo.lastModified();
    }
  }

  // tess-eval shader
  if (_tessEvalShaderFile)
  {
    fileInfo = QFileInfo(_tessEvalShaderFile);
    if (fileInfo.isRelative())
    {
      QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_tessEvalShaderFile);
      fileInfo = QFileInfo(absFilename);

      newEntry.strTessEvaluationTemplate = absFilename;
      newEntry.tessEvaluationFileLastMod = fileInfo.lastModified();
    }
    else
    {
      newEntry.strTessEvaluationTemplate = _tessEvalShaderFile;
      newEntry.tessEvaluationFileLastMod = fileInfo.lastModified();
    }
  }



  if (_macros)
    newEntry.macros = *_macros;

  CacheList::iterator oldCache = cacheStatic_.end();

  for (CacheList::iterator it = cacheStatic_.begin(); it != cacheStatic_.end();  ++it)
  {
    // If the shaders are equal, we return the cached entry
    if (!compareShaderGenDescs(&it->first, &newEntry))
    {
      if ( timeCheck_ && !compareTimeStamp(&it->first, &newEntry))
        oldCache = it;
      else
        return it->second;
    }
  }


  // convert QStringList to GLSL::StringList

  GLSL::StringList glslMacros;

  if (_macros)
  {
    for (QStringList::const_iterator it = _macros->constBegin(); it != _macros->constEnd(); ++it)
      glslMacros.push_back(it->toStdString());
  }


  GLSL::Program* prog = GLSL::loadProgram(_vertexShaderFile, _tessControlShaderFile, _tessEvalShaderFile, _geometryShaderFile, _fragmentShaderFile, &glslMacros, _verbose);
  glCheckErrors();

  if (oldCache != cacheStatic_.end())
  {
    if (!prog || !prog->isLinked())
    {
      delete prog;
      return oldCache->second;
    }
    else
    {
      cacheStatic_.erase(oldCache);
    }
  }

  cacheStatic_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

  return prog;
}

GLSL::Program* ACG::ShaderCache::getProgram( const char* _vertexShaderFile, const char* _fragmentShaderFile, QStringList* _macros, bool _verbose )
{
  return getProgram(_vertexShaderFile, 0, 0, 0, _fragmentShaderFile, _macros, _verbose);
}

GLSL::Program* ACG::ShaderCache::getComputeProgram(const char* _computeShaderFile, QStringList* _macros /* = 0 */, bool _verbose /* = true */)
{
  CacheEntry newEntry;

  // store filenames and timestamps in new entry
  // use vertex shader filename as compute shader
  QFileInfo fileInfo(_computeShaderFile);
  if (fileInfo.isRelative())
  {
    QString absFilename = ACG::ShaderProgGenerator::getShaderDir() + QDir::separator() + QString(_computeShaderFile);
    fileInfo = QFileInfo(absFilename);

    newEntry.strVertexTemplate = absFilename;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }
  else
  {
    newEntry.strVertexTemplate = _computeShaderFile;
    newEntry.vertexFileLastMod = fileInfo.lastModified();
  }

  if (_macros)
    newEntry.macros = *_macros;

  CacheList::iterator oldCache = cacheComputeShaders_.end();

  for (CacheList::iterator it = cacheComputeShaders_.begin(); it != cacheComputeShaders_.end();  ++it)
  {
    // If the shaders are equal, we return the cached entry
    if (!compareShaderGenDescs(&it->first, &newEntry))
    {
      if ( ( timeCheck_ && !compareTimeStamp(&it->first, &newEntry)) || !it->second || !it->second->isLinked())
        oldCache = it;
      else
        return it->second;
    }
  }


  // convert QStringList to GLSL::StringList

  GLSL::StringList glslMacros;

  if (_macros)
  {
    for (QStringList::const_iterator it = _macros->constBegin(); it != _macros->constEnd(); ++it)
      glslMacros.push_back(it->toStdString());
  }


  GLSL::Program* prog = GLSL::loadComputeProgram(newEntry.strVertexTemplate.toUtf8(), &glslMacros, _verbose);
  glCheckErrors();

  if (oldCache != cacheComputeShaders_.end())
  {
    if (!prog || !prog->isLinked())
    {
      delete prog;


      // dump shader source including macros to debug output
      if (!glslMacros.empty() && !dbgOutputDir_.isEmpty())
      {
        GLSL::StringList shaderSrc = GLSL::loadShader(newEntry.strVertexTemplate.toUtf8(), &glslMacros);

        // compute FNV hash of macros

        unsigned int fnv_prime = 16777619;
        unsigned int fnv_hash = 2166136261;

        for (GLSL::StringList::iterator it = shaderSrc.begin(); it != shaderSrc.end(); ++it)
        {
          for (size_t i = 0; i < it->length(); ++i)
          {
            fnv_hash *= fnv_prime;
            fnv_hash ^= static_cast<unsigned char>((*it)[i]);
          }
        }

        QString fnv_string = QString::number(fnv_hash,16).toUpper();

        std::string dumpFilename = QString(dbgOutputDir_ + QDir::separator() + fileInfo.fileName() + fnv_string).toStdString();

        std::ofstream dumpStream(dumpFilename.c_str());
        if (dumpStream.is_open())
        {
          for (GLSL::StringList::iterator it = shaderSrc.begin(); it != shaderSrc.end(); ++it)
            dumpStream << it->c_str();
          dumpStream.close();
        }
      }

      return oldCache->second;
    }
    else
    {
      cacheComputeShaders_.erase(oldCache);
    }
  }

  cacheComputeShaders_.push_back(std::pair<CacheEntry, GLSL::Program*>(newEntry, prog));

  return prog;
}

bool ACG::ShaderCache::compareTimeStamp(const CacheEntry* _a, const CacheEntry* _b)
{
  if (_a->vertexFileLastMod != _b->vertexFileLastMod)
    return false;

  if (_a->geometryFileLastMod != _b->geometryFileLastMod)
    return false;

  if (_a->fragmentFileLastMod != _b->fragmentFileLastMod)
    return false;

  if (_a->tessControlFileLastMod != _b->tessControlFileLastMod)
    return false;

  if (_a->tessEvaluationFileLastMod != _b->tessEvaluationFileLastMod)
    return false;
  return true;
}

int ACG::ShaderCache::compareShaderGenDescs( const CacheEntry* _a, const CacheEntry* _b)
{
  if (_a->mods != _b->mods)
    return -1;

  const ShaderGenDesc* a = &_a->desc;
  const ShaderGenDesc* b = &_b->desc;

  if (_a->strFragmentTemplate != _b->strFragmentTemplate)
    return -1;

  if (_a->strGeometryTemplate != _b->strGeometryTemplate)
      return -1;

  if (_a->strVertexTemplate != _b->strVertexTemplate)
    return -1;

  if (_a->strTessControlTemplate != _b->strTessControlTemplate)
    return -1;

  if (_a->strTessEvaluationTemplate != _b->strTessEvaluationTemplate)
    return -1;

  if (_a->macros != _b->macros)
    return -1;


  return *a == *b ? 0 : -1;
}


void ACG::ShaderCache::clearCache()
{
  cache_.clear();
  cacheStatic_.clear();
  cacheComputeShaders_.clear();
}

void ACG::ShaderCache::setDebugOutputDir(const char* _outputDir)
{
  dbgOutputDir_ = _outputDir;
}

//=============================================================================
} // namespace ACG
//=============================================================================
