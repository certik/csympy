require 'spec_helper'

describe SymEngine do
  before :each do
  end

  describe Basic do
    before :each do
      @basic = Basic.new
    end

    describe "#new" do
      it "takes no parameters and returns a Basic object" do
        @basic.should be_an_instance_of Basic
      end
    end

  end
end
