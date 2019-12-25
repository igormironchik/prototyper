# About

Prototyper this is desktop application to make UI prototypes, simple integration
into project's documentation, and so on...

First of all I have to say that I see Prototyper as a software for creation of UI
prototypes for desktop and mobile applications. I do not set a goal to create prototyping
software for Web-pages, but may be Prototyper will be usefull for Web-pages too.

When you first time launch Prototyper you will see window with text editor (one and alone)
tab and two dock widgets. This tabbed text editor is the place where you can describe
your new project. Top dock widget is a tree view with hierarchy of items on the forms.
And bottom dock widget is tabs navigation.

Form can be created through the context menu on the tab, context menu in tabs navigation
or through the menu. When you create form you will see tool bar with tools for drawing.

In the tree view with hierarchy of forms' items you can select items (navigate
through the forms), rename items, set items' links to another forms and create
descriptions for the items (elements) on the form, this descriptions will be
used to generate documentation.

In such a way you describe prototype of you project. Once as you are ready
you can export your project to the PDF, HTML or SVG images of forms.

And Prototyper released under GNU GPL v3.0!!!

# Some screenshots
            
![Description page]( https://image.ibb.co/byzBT5/prototyper_description.png )
            
![Form page]( https://image.ibb.co/joud85/prototyper_dialog.png )

# Getting from Repository

After clone update submodules with the next command:

```
git submodule update --init --recursive
```

# Roadmap

First version of the Prototyper uses forms, form items hierarchy, descriptions
of the items. It's very hard to create mockups with this application. I decided
to refuse from forms, and create second version with pages and comments, like
in [Moqups](https://moqups.com/). I believe that this approach will be simpler a
lot.
