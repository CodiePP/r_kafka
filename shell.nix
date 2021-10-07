{ pkgs ? import <nixpkgs> {}
}:

  pkgs.rstudioWrapper.override {
        packages = with pkgs.rPackages; 
          [ pkgs.gcc
            pkgs.postgresql
            pkgs.openssl
            pkgs.git
            pkgs.libgit2
            pkgs.libxml2
            pkgs.zlib
            pkgs.pkg-config
            pkgs.rdkafka
          ] ++ 
          [ dplyr
            ggplot2 ggthemes
            jsonlite
            reshape2 
            httr plotly 
            shiny shinyjs shinythemes
            DT
            #RPostgres
            #DBI
            xml2
            devtools
          ];
  }


