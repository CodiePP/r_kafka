{ pkgs ? import <nixpkgs> {}
}:

  pkgs.rstudioWrapper.override {
        packages = with pkgs.rPackages; 
          [ dplyr
            ggplot2 ggthemes
            jsonlite
            reshape2 
            httr plotly 
            shiny shinyjs shinythemes
            DT
            RPostgres
            DBI
            devtools
          ] ++ 
          [ pkgs.gcc
            pkgs.postgresql
            pkgs.openssl
            pkgs.git
            pkgs.zlib
            pkgs.pkg-config
            pkgs.rdkafka
          ];
  }


