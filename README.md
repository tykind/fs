### FS
File system wrapper, supported only with c++20.

## Usage

* **Folders**
```cpp
fs::Folder cfg("cfg");

if (cfg.exists)
{

  for (const auto& file : cfg.read())
     if (file->getFileType() == fs::enums::DOCUMENT)
     {
          const auto document = file->translate<fs::Document>();

          // Log file & content

          std::clog << "Path: " << document->path << "\nContents: " << document->read().str().c_str() << std::endl;

          std::stringstream toWrite{ "New contents" };
          document->write(toWrite);
     }
}
```

* **Normal files**
```cpp
fs::Document document("cfg/not ignored 1")

// Log document content

std::clog << document->read().str().c_str() << std::endl;
```

## Image example
![image](https://user-images.githubusercontent.com/106443697/199407090-a12f4f1b-6599-4e5e-913a-0d2696478af0.png)
